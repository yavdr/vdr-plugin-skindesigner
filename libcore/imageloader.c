#include "../config.h"
#include "helpers.h"
#include "imageloader.h"
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

    cairo_status_t status = cairo_status(cr);
    if (status)
        dsyslog("skindesigner: Cairo CreateImage Error %s", cairo_status_to_string(status));

    unsigned char *data = cairo_image_surface_get_data(surface);
    cImage *image = new cImage(cSize(width, height), (tColor*)data);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

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
    else if (endswith(fullpath, ".svg"))
        importer = new cImageImporterSVG;
    else if (endswith(fullpath, ".jpg"))
        importer = new cImageImporterJPG;
    else
        return false;

    return importer->LoadImage(fullpath);
}

// Just a different way to call LoadImage. Calls the above one.
bool cImageLoader::LoadImage(std::string Path, std::string FileName, std::string Extension) {
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
    if (surface) {
        cairo_set_source_surface(cr, surface, 0, 0);
        cairo_paint(cr);
    }
}

void cImageImporterPNG::GetImageSize(int &width, int &height) {
    if (surface) {
        width = cairo_image_surface_get_width(surface);
        height = cairo_image_surface_get_height(surface);
    }
}

//
// Image importer for SVG
//

cImageImporterSVG::cImageImporterSVG() {
    handle = NULL;
}

cImageImporterSVG::~cImageImporterSVG() {
    if (handle) {
        rsvg_handle_close(handle, NULL);
        g_object_unref(handle);
    }
}

bool cImageImporterSVG::LoadImage(const char *path) {
    if (handle) {
        rsvg_handle_close(handle, NULL);
        g_object_unref(handle);
    }

    GError *error = NULL;
    handle = rsvg_handle_new_from_file(path, &error);
    if (!handle) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: RSVG Error: %s", error->message);
        return false;
    }

    rsvg_handle_set_dpi(handle, 90);

    return true;
}

void cImageImporterSVG::DrawToCairo(cairo_t *cr) {
    if (handle)
        rsvg_handle_render_cairo(handle, cr);
}

void cImageImporterSVG::GetImageSize(int &width, int &height) {
    if (handle) {
        RsvgDimensionData dim;
        rsvg_handle_get_dimensions(handle, &dim);
        width = dim.width;
        height = dim.height;
    }
}

//
// Image importer for JPG
//

cImageImporterJPG::cImageImporterJPG() {
    cinfo = NULL;
}

cImageImporterJPG::~cImageImporterJPG() {
    if (cinfo) {
        jpeg_destroy_decompress(cinfo);
        free(cinfo);
        fclose(infile);
    }
}

bool cImageImporterJPG::LoadImage(const char *path) {
    if (cinfo) {
        jpeg_destroy_decompress(cinfo);
        free(cinfo);
        fclose(infile);
        cinfo = NULL;
    }

    // Open input file
    if ((infile = fopen(path, "rb")) == NULL) {
        if (config.debugImageLoading)
            dsyslog("skindesigner: Can't open %s", path);
        return false;
    }

    // We set up the normal JPEG error routines, then override error_exit.
    struct my_error_mgr jerr;
    cinfo = (j_decompress_ptr)malloc(sizeof(struct jpeg_decompress_struct));
    cinfo->err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message = my_output_message;
    // Establish the setjmp return context for my_error_exit to use.
    if (setjmp(jerr.setjmp_buffer)) {
        // If we get here, the JPEG code has signaled an error.
        jpeg_destroy_decompress(cinfo);
        free(cinfo);
        fclose(infile);
        cinfo = NULL;
        return false;
    }

    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(cinfo);

    // Step 2: specify data source (eg, a file)
    jpeg_stdio_src(cinfo, infile);

    // Step 3: read file parameters with jpeg_read_header()
    (void) jpeg_read_header(cinfo, TRUE);
    return true;
}

void cImageImporterJPG::DrawToCairo(cairo_t *cr) {
    if (!cinfo)
        return;

    unsigned char *bmp_buffer = NULL;

    // Re-establish error handling
    struct my_error_mgr jerr;
    cinfo->err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message = my_output_message;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(cinfo);
        free(cinfo);
        fclose(infile);
        free(bmp_buffer);
        cinfo = NULL;
        return;
    }

    // Step 4: set parameters for decompression
    cinfo->out_color_space = JCS_EXT_ARGB;

    // Step 5: Start decompressor
    (void) jpeg_start_decompress(cinfo);

    // Bytes per row in output buffer
    int row_stride = cinfo->output_width * cinfo->output_components;

    // Allocate buffer
    unsigned long bmp_size = row_stride * cinfo->output_height;
    bmp_buffer = (unsigned char*)malloc(bmp_size);

    // Step 6: while (scan lines remain to be read)
    while (cinfo->output_scanline < cinfo->output_height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo->output_scanline) * row_stride;
        jpeg_read_scanlines(cinfo, buffer_array, 1);
    }

    // Step 7: Finish decompression
    (void)jpeg_finish_decompress(cinfo);
    fclose(infile);

    // --> At this point we have raw RGB data in bmp_buffer

    // Create new Cairo surface from our raw image data
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_for_data(bmp_buffer,
                                                  CAIRO_FORMAT_ARGB32,
                                                  cinfo->output_width,
                                                  cinfo->output_height,
                                                  row_stride);

    // Draw surface to Cairo
    if (surface) {
        cairo_set_source_surface(cr, surface, 0, 0);
        cairo_paint(cr);
        cairo_surface_destroy(surface);
    }

    // Cleanup. In this "ImageImporter" we clean up everything in "DrawToCairo"
    // as I'm not really sure whether we are able to draw a second time.
    free(bmp_buffer);
    jpeg_destroy_decompress(cinfo);
    free(cinfo);
    cinfo = NULL;
}

void cImageImporterJPG::GetImageSize(int &width, int &height) {
    if (cinfo) {
        width = cinfo->image_width;
        height = cinfo->image_height;
    }
}
