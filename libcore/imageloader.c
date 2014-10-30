#include "../config.h"
#include "helpers.h"
#include "imageloader.h"
//#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

cImageLoader::cImageLoader() {
    importer = NULL;
}

cImageLoader::~cImageLoader() {
    delete(importer);
}

cImage *cImageLoader::CreateImage(int width, int height, bool preserveAspect) {
    if (!importer) return NULL;

    int w, h;
    importer->GetImageSize(w, h);
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

    importer->DrawToCairo(cr);
    cairo_paint(cr);

    cairo_status_t status = cairo_status(cr);
    if (status)
        dsyslog("skindesigner: Cairo CreateImage Error %s", cairo_status_to_string(status));

    unsigned char *data = cairo_image_surface_get_data(surface);
    cImage *image = new cImage(cSize(width, height), (tColor*)data);

    cairo_destroy(cr);

    return image;
}

bool cImageLoader::LoadImage(const char *fullpath) {
    if ((fullpath == NULL) || (strlen(fullpath) < 5))
        return false;

    if (config.debugImageLoading)
        dsyslog("skindesigner: trying to load: %s", fullpath);

    delete(importer);
    importer = NULL;

    if (endswith(fullpath, ".png"))
        importer = new cImageImporterPNG;
    else
        return false;

    return importer->LoadImage(fullpath);
}

// Just a different way to call LoadImage. Calls the above one.
bool cImageLoader::LoadImage(std::string FileName, std::string Path, std::string Extension) {
    std::stringstream sstrImgFile;
    sstrImgFile << Path << FileName << "." << Extension;
    std::string imgFile = sstrImgFile.str();
    return LoadImage(imgFile.c_str());
}

void cImageLoader::DeterminateChannelLogoSize(int &width, int &height) {
    cString logoPath;
    cString logoPathSkin = cString::sprintf("%s%s/themes/%s/logos/", *config.skinPath, Setup.OSDSkin, Setup.OSDTheme);
    if (FolderExists(*logoPathSkin)) {
        logoPath = logoPathSkin;
    } else {
        logoPath = config.logoPath;
    }
    cString logoExt = config.logoExtension;
    DIR *folder = NULL;
    struct dirent *file;
    folder = opendir(logoPath);
    if (!folder) {
        return;
    }
    while (file = readdir(folder)) {
        if (endswith(file->d_name, *logoExt)) {
            std::stringstream filePath;
            filePath << *logoPath << file->d_name;
            if (LoadImage(filePath.str().c_str())) {
                int logoWidth = 0;
                int logoHeight = 0;
                importer->GetImageSize(logoWidth, logoHeight);
                if (logoWidth > 0 && logoHeight > 0) {
                    width = logoWidth;
                    height = logoHeight;
                    delete(importer);
                    importer = NULL;
                    return;
                }
            }
        }
    }
}


//
// Image importer for PNG
//

cImageImporterPNG::cImageImporterPNG() {
    surface = NULL;
}

cImageImporterPNG::~cImageImporterPNG() {
    if (surface)
        cairo_surface_destroy(surface);
}

bool cImageImporterPNG::LoadImage(const char *path) {
    if (surface)
        cairo_surface_destroy(surface);

    surface = cairo_image_surface_create_from_png(path);

    if (cairo_surface_status(surface)) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Cairo LoadImage Error: %s", cairo_status_to_string(cairo_surface_status(surface)));
        surface = NULL;
        return false;
    }

    return true;
}

void cImageImporterPNG::DrawToCairo(cairo_t *cr) {
    if (surface)
        cairo_set_source_surface(cr, surface, 0, 0);
}

void cImageImporterPNG::GetImageSize(int &width, int &height) {
    if (surface) {
        width = cairo_image_surface_get_width(surface);
        height = cairo_image_surface_get_height(surface);
    }
}
