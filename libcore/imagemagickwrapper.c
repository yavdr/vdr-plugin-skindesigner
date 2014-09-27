#include <string>
#include <sstream>
#include "imagemagickwrapper.h"
#include "../config.h"
#include "imagescaler.h"

cImageMagickWrapper::cImageMagickWrapper() {
    InitializeMagick(NULL);
}

cImageMagickWrapper::~cImageMagickWrapper() {
}

cImage *cImageMagickWrapper::CreateImage(int width, int height, bool preserveAspect) {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    if (width == 0)
        width = w;
    if (height == 0)
        height = h;
    if (preserveAspect) {
        unsigned scale_w = 1000 * width / w;
        unsigned scale_h = 1000 * height / h;
        if (scale_w > scale_h)
          width = w * height / h;
        else
          height = h * width / w;
    }
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    cImage *image = new cImage(cSize(width, height));
    tColor *imgData = (tColor *)image->Data();
    if (w != width || h != height) {
        ImageScaler scaler;
        scaler.SetImageParameters(imgData, width, width, height, w, h);
        for (const void *pixels_end = &pixels[w*h]; pixels < pixels_end; ++pixels)
            scaler.PutSourcePixel(pixels->blue / ((MaxRGB + 1) / 256),
                                  pixels->green / ((MaxRGB + 1) / 256),
                                  pixels->red / ((MaxRGB + 1) / 256),
                                  ~((unsigned char)(pixels->opacity / ((MaxRGB + 1) / 256))));
        return image;
    }
    for (const void *pixels_end = &pixels[width*height]; pixels < pixels_end; ++pixels)
        *imgData++ = ((~int(pixels->opacity / ((MaxRGB + 1) / 256)) << 24) |
                      (int(pixels->green / ((MaxRGB + 1) / 256)) << 8) |
                      (int(pixels->red / ((MaxRGB + 1) / 256)) << 16) |
                      (int(pixels->blue / ((MaxRGB + 1) / 256)) ));
    return image;
}

cImage cImageMagickWrapper::CreateImageCopy() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage image (cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
        for (int ix = 0; ix < w; ++ix) {
            tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24)
            | (int(pixels->green * 255 / MaxRGB) << 8)
            | (int(pixels->red * 255 / MaxRGB) << 16)
            | (int(pixels->blue * 255 / MaxRGB) );
            image.SetPixel(cPoint(ix, iy), col);
            ++pixels;
        }
    }
    return image;
}

bool cImageMagickWrapper::LoadImage(std::string FileName, std::string Path, std::string Extension) {
    try {
        std::stringstream sstrImgFile;
        sstrImgFile << Path << FileName << "." << Extension;
        std::string imgFile = sstrImgFile.str();
        if (config.debugImageLoading)
            dsyslog("skindesigner: trying to load: %s", imgFile.c_str());
        buffer.read(imgFile.c_str());
        if (config.debugImageLoading)
            dsyslog("skindesigner: %s sucessfully loaded", imgFile.c_str());
    } catch( Magick::Warning &warning ) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Magick Warning: %s", warning.what());
        return true;
    } catch( Magick::Error &error ) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Magick Error: %s", error.what());
        return false;
    } catch(...) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: an unknown Magick error occured during image loading");
        return false;
    }
    return true;
}

bool cImageMagickWrapper::LoadImage(const char *fullpath) {
    if ((fullpath == NULL) || (strlen(fullpath) < 5))
        return false;
    try {
        if (config.debugImageLoading)
            dsyslog("skindesigner: trying to load: %s", fullpath);
        buffer.read(fullpath);
        if (config.debugImageLoading)
            dsyslog("skindesigner: %s sucessfully loaded", fullpath);
    } catch( Magick::Warning &warning ) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Magick Warning: %s", warning.what());
        return true;
    } catch( Magick::Error &error ) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Magick Error: %s", error.what());
        return false;
    } catch(...) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: an unknown Magick error occured during image loading");
        return false;
    }
    return true;
}

Color cImageMagickWrapper::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}

void cImageMagickWrapper::CreateGradient(tColor back, tColor blend, int width, int height, double wfactor, double hfactor) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    int maxw = MaxRGB * wfactor;
    int maxh = MaxRGB * hfactor;

    Image imgblend(Geometry(width, height), Blend);
    imgblend.modifyImage();
    imgblend.type(TrueColorMatteType);
    PixelPacket *pixels = imgblend.getPixels(0, 0, width, height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            PixelPacket *pixel = pixels + y * width + x;
            int opacity = (maxw / width * x + maxh - maxh / height * y) / 2;
            pixel->opacity = (opacity <= MaxRGB) ? opacity : MaxRGB;
        }
    }
    imgblend.syncPixels();

    Image imgback(Geometry(width, height), Back);
    imgback.composite(imgblend, 0, 0, OverCompositeOp);

    buffer = imgback;
}

void cImageMagickWrapper::CreateBackground(tColor back, tColor blend, int width, int height, bool mirror) {
    CreateGradient(back, blend, width, height, 0.8, 0.8);
    if (mirror)
        buffer.flop();
}
void cImageMagickWrapper::CreateBackgroundReverse(tColor back, tColor blend, int width, int height) {
    CreateGradient(back, blend, width, height, 1.3, 0.7);
}
