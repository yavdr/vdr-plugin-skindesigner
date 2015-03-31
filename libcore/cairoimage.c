#include "cairoimage.h"
#include "../libtemplate/templatefunction.h"

cCairoImage::cCairoImage(void) {
    surface = NULL;
    cr = NULL;
}

cCairoImage::~cCairoImage() {
    if (cr)
        cairo_destroy (cr);
    if (surface)
        cairo_surface_destroy (surface);
}

void cCairoImage::InitCairoImage(int width, int height) {
    this->width = width;
    this->height = height;
    
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create(surface);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);
}

void cCairoImage::DrawTextVertical(string text, tColor color, string font, int size, int direction) {

    int imgHeight = GetTextWidth(text, font, size);
    InitCairoImage(size * 1.2, imgHeight);

    SetColor(color);
    cairo_font_weight_t fontWeight = CAIRO_FONT_WEIGHT_NORMAL;
    cairo_font_slant_t fontSlant = CAIRO_FONT_SLANT_NORMAL;
    cairo_select_font_face (cr, font.c_str(), fontSlant, fontWeight);
    cairo_set_font_size (cr, size);
    int x = size;
    int y = imgHeight;
    double rotate = 3*M_PI/2;
    if (direction == diTopDown) {
        rotate = M_PI/2;
        x = size*0.3;
        y = 0;
    }
    cairo_move_to (cr, x, y);
    cairo_rotate(cr, rotate);
    cairo_show_text (cr, text.c_str());
}

cImage *cCairoImage::GetImage(void) {
    if (!cr || !surface)
       return NULL;

    unsigned char *data = cairo_image_surface_get_data(surface);
    cImage *image = new cImage(cSize(width, height), (tColor*)data);
    return image;
}

/**********************************************************************************
* Private Functions
**********************************************************************************/

int cCairoImage::GetTextWidth(string text, string font, int size) {
    cairo_surface_t *tmpSurface;
    cairo_t *tmpCr;

    double width = 300;
    double height = (double)size *1.3;

    cairo_font_weight_t fontWeight = CAIRO_FONT_WEIGHT_NORMAL;
    cairo_font_slant_t fontSlant = CAIRO_FONT_SLANT_NORMAL;

    tmpSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    tmpCr = cairo_create (tmpSurface);

    cairo_select_font_face (tmpCr, font.c_str(), fontSlant, fontWeight);
    cairo_set_font_size (tmpCr, size);
    
    cairo_text_extents_t te;
    cairo_text_extents (tmpCr, text.c_str(), &te);
    int textWidth = te.width;

    cairo_destroy (tmpCr);
    cairo_surface_destroy (tmpSurface);

    return (double)textWidth * 1.1;
}

void cCairoImage::SetColor(tColor color) {
    if (!cr || !surface)
        return;
    tIndex tAlpha = (color & 0xFF000000) >> 24;
    tIndex tRed = (color & 0x00FF0000) >> 16;
    tIndex tGreen = (color & 0x0000FF00) >> 8;
    tIndex tBlue = (color & 0x000000FF);

    double a = (int)tAlpha / (double)255;
    double r = (int)tRed / (double)255;
    double g = (int)tGreen / (double)255;
    double b = (int)tBlue / (double)255;

    cairo_set_source_rgba(cr, r, g, b, a);
}
