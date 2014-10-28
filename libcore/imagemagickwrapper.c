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
    if (image == NULL) return NULL;

    int w, h;
    w = cairo_image_surface_get_width(image);
    h = cairo_image_surface_get_height(image);
    if (width == 0)
        width = w;
    if (height == 0)
        height = h;

    cairo_surface_t *surface;
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

    cairo_t *cr;
    cr = cairo_create(surface);

    double sx = width / (double)w;
    double sy = height / (double)h;
    if (preserveAspect) {
        if (sx < sy)
            sy = sx;
        if (sy < sx)
            sx = sy;
    }
    cairo_scale(cr, sx, sy);

    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);

    cairo_status_t status = cairo_status (cr);
    if (status)
        dsyslog("skindesigner: Cairo CreateImage Error %s", cairo_status_to_string(status));

    unsigned char *data = cairo_image_surface_get_data(surface);
    cImage *cimage = new cImage(cSize(width, height), (tColor*)data);

    cairo_destroy(cr);
    cairo_surface_destroy(image);
    image = NULL;

    return cimage;
}

bool cImageMagickWrapper::LoadImage(const char *fullpath) {
    if ((fullpath == NULL) || (strlen(fullpath) < 5))
        return false;

    if (image != NULL) cairo_surface_destroy(image);

    if (config.debugImageLoading)
        dsyslog("skindesigner: trying to load: %s", fullpath);

    image = cairo_image_surface_create_from_png(fullpath);

    if (cairo_surface_status(image)) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Cairo LoadImage Error: %s", cairo_status_to_string(cairo_surface_status(image)));
        image = NULL;
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
