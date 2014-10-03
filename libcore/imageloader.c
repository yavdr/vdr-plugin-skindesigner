#include "../config.h"
#include "helpers.h"
#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

using namespace Magick;

cImageLoader::cImageLoader() : cImageMagickWrapper() {
}

cImageLoader::~cImageLoader() {
}

cImage cImageLoader::GetImage() {
    return CreateImageCopy();
}

bool cImageLoader::LoadImage(const char *path, int width, int height) {
    if (cImageMagickWrapper::LoadImage(path)) {
        buffer.sample(Geometry(width, height));
        return true;
    }
    return false;
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
            Image logo;
            try {
                logo.read(filePath.str().c_str());
                Geometry g = logo.size();
                int logoWidth = g.width();
                int logoHeight = g.height();
                if (logoWidth > 0 && logoHeight > 0) {
                    width = logoWidth;
                    height = logoHeight;
                    return;
                }
            } catch( ... ) { }
        }
    }
}
