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

// Just a different way to call LoadImage. Calls the above one.
bool cImageMagickWrapper::LoadImage(std::string FileName, std::string Path, std::string Extension) {
    std::stringstream sstrImgFile;
    sstrImgFile << Path << FileName << "." << Extension;
    std::string imgFile = sstrImgFile.str();
    return LoadImage(imgFile.c_str());
}
