#ifndef __NOPACITY_IMAGELOADER_H
#define __NOPACITY_IMAGELOADER_H

#define X_DISPLAY_MISSING

#include <cairo.h>
#include <librsvg/rsvg.h>
#include <vdr/osd.h>
#include <vdr/tools.h>

//
// Image importers
//
class cImageImporter {
public:
    cImageImporter() {};
    virtual ~cImageImporter() {};
    virtual bool LoadImage(const char *path) {};
    virtual void DrawToCairo(cairo_t *cr) {};
    virtual void GetImageSize(int &width, int &height) {};
};

// Image importer for PNG
class cImageImporterPNG : public cImageImporter {
public:
    cImageImporterPNG();
    ~cImageImporterPNG();
    bool LoadImage(const char *path);
    void DrawToCairo(cairo_t *cr);
    void GetImageSize(int &width, int &height);
private:
    cairo_surface_t *surface;
};

// Image importer for SVG
class cImageImporterSVG : public cImageImporter {
public:
    cImageImporterSVG();
    ~cImageImporterSVG();
    bool LoadImage(const char *path);
    void DrawToCairo(cairo_t *cr);
    void GetImageSize(int &width, int &height);
private:
    RsvgHandle *handle;
};


class cImageLoader {
private:
    cImageImporter *importer = NULL;
public:
    cImageLoader();
    virtual ~cImageLoader();
    cImage *CreateImage(int width, int height, bool preserveAspect = true);
    bool LoadImage(std::string Path, std::string FileName, std::string Extension);
    bool LoadImage(const char *fullpath);
    void DeterminateChannelLogoSize(int &width, int &height);
};

#endif //__NOPACITY_IMAGELOADER_H
