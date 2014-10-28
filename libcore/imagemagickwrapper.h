#ifndef __NOPACITY_IMAGEMAGICKWRAPPER_H
#define __NOPACITY_IMAGEMAGICKWRAPPER_H

#define X_DISPLAY_MISSING

#include <Magick++.h>
#include <vdr/osd.h>

using namespace Magick;

class cImageMagickWrapper {
private:
public:
    cImageMagickWrapper();
    ~cImageMagickWrapper();
protected:
    Image buffer;
    cImage *CreateImage(int width, int height, bool preserveAspect = true);
    bool LoadImage(std::string FileName, std::string Path, std::string Extension);
    bool LoadImage(const char *fullpath);
};

#endif //__NOPACITY_IMAGEMAGICKWRAPPER_H
