#ifndef __CAIROIMAGE_H
#define __CAIROIMAGE_H

#include <cairo.h>
#include <vdr/osd.h>
#include <string>
#include <sstream>

using namespace std;

class cCairoImage {
private:
    int width;
    int height;
    cairo_surface_t *surface;
    cairo_t *cr;
    void SetColor(tColor color);
    int GetTextWidth(string text, string font, int size);
public:
    cCairoImage(void);
    virtual ~cCairoImage();
    void InitCairoImage(int width, int height);
    void DrawTextVertical(string text, tColor color, string font, int size);
    cImage *GetImage(void);
};
#endif //__CAIROIMAGE_H