#ifndef __NOPACITY_IMAGEMAGICKWRAPPER_H
#define __NOPACITY_IMAGEMAGICKWRAPPER_H

#define X_DISPLAY_MISSING

#include <Magick++.h>
#include <vdr/osd.h>

using namespace Magick;

class cImageMagickWrapper {
private:
    void CreateGradient(tColor back, tColor blend, int width, int height, double wfactor, double hfactor);
public:
    cImageMagickWrapper();
    ~cImageMagickWrapper();
protected:
    Image buffer;
    Color Argb2Color(tColor col);
    cImage *CreateImage(int width, int height, bool preserveAspect = true);
    cImage CreateImageCopy(void);
    bool LoadImage(std::string FileName, std::string Path, std::string Extension);
    bool LoadImage(const char *fullpath);
    void CreateBackground(tColor back, tColor blend, int width, int height, bool mirror = false);
    void CreateBackgroundReverse(tColor back, tColor blend, int width, int height);
};

#endif //__NOPACITY_IMAGEMAGICKWRAPPER_H
