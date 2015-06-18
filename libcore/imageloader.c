#include "../config.h"
#include "helpers.h"
#include "imageloader.h"
#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>

cImageLoader::cImageLoader() {
    importer = NULL;
}

cImageLoader::~cImageLoader() {
    delete(importer);
}

cImage *cImageLoader::CreateImage(int width, int height, bool preserveAspect) {
    if (!importer)
        return NULL;
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
        double tx = 0;
        double ty = 0;
        if (sx < sy) {
            sy = sx;
            ty = (height - h * sy) / 2;
        }
        if (sy < sx) {
            sx = sy;
            tx = (width - w * sx) / 2;
        }
        cairo_translate(cr, tx, ty);
    }
    cairo_scale(cr, sx, sy);

    importer->DrawToCairo(cr);

    cairo_status_t status = cairo_status(cr);
    if (status && config.debugImageLoading)
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
    else {
        importer = cImageImporter::CreateImageImporter(fullpath);
        if (!importer)
            return false;
    }

    return importer->LoadImage(fullpath);
}

// Just a different way to call LoadImage. Calls the above one.
bool cImageLoader::LoadImage(std::string Path, std::string FileName, std::string Extension) {
    std::stringstream sstrImgFile;
    sstrImgFile << Path << FileName << "." << Extension;
    std::string imgFile = sstrImgFile.str();
    return LoadImage(imgFile.c_str());
}

cImageImporter* cImageImporter::CreateImageImporter(const char* path) {
    char pngSig[] = { char(0x89), char(0x50), char(0x4E), char(0x47), char(0x0D), char(0x0A), char(0x1A), char(0x0A) };
    char jpgSig[] = { char(0xFF), char(0xD8), char(0xFF), char(0xD9) };
    char buffer[8] = { 0 };
    ifstream f(path, ios::in | ios::binary);
    f.read(buffer, 8);
    if (!f)
        return NULL;
    if(buffer[0] == jpgSig[0] && buffer[1] == jpgSig[1]) {
        f.seekg(-2, f.end);
        f.read(buffer, 2);
        if(buffer[0] == jpgSig[2] && buffer[1] == jpgSig[3]) {
            f.close();
            return new cImageImporterJPG;
        }
    } else if(buffer[0] == pngSig[0] 
        && buffer[1] == pngSig[1]
        && buffer[2] == pngSig[2]
        && buffer[3] == pngSig[3]
        && buffer[4] == pngSig[4]
        && buffer[5] == pngSig[5]
        && buffer[6] == pngSig[6]
        && buffer[7] == pngSig[7]) {
            f.close();
            return new cImageImporterPNG;
    } 
    f.close();
    return NULL;
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
        if (config.debugImageLoading && error) {
            dsyslog("skindesigner: RSVG Error: %s", error->message);
        }
        return false;
    }

    // 90 dpi is the hardcoded default setting of the Inkscape SVG editor
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

void cImageImporterSVG::InitLibRSVG() {
    #if !GLIB_CHECK_VERSION(2, 35, 0)
        g_type_init();
    #endif
}

//
// Image importer for JPG
//

struct my_error_mgr {
    struct jpeg_error_mgr pub; // "public" fields
    jmp_buf setjmp_buffer;     // for return to caller
};

METHODDEF(void)
my_error_exit(j_common_ptr cinfo) {
    // cinfo->err really points to a my_error_mgr struct, so coerce pointer
    my_error_mgr *myerr = (my_error_mgr*) cinfo->err;

    // Always display the message.
    (*cinfo->err->output_message) (cinfo);

    // Return control to the setjmp point
    longjmp(myerr->setjmp_buffer, 1);
}

METHODDEF(void)
my_output_message(j_common_ptr cinfo) {
    char buf[JMSG_LENGTH_MAX];
    cinfo->err->format_message(cinfo, buf);
    if (config.debugImageLoading)
        dsyslog("skindesigner: libjpeg error: %s", buf);
}

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

    // Allocate space for our decompress struct
    cinfo = (j_decompress_ptr)malloc(sizeof(struct jpeg_decompress_struct));

    // We set up the normal JPEG error routines, then override error_exit
    // and output_message.
    struct my_error_mgr jerr;
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

    // Re-establish error handling. We have to do this again as the saved
    // calling environment of "LoadImage" is invalid if we reach here!
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
    cinfo->out_color_space = JCS_RGB;

    // Step 5: Start decompressor
    (void) jpeg_start_decompress(cinfo);

    // Allocate buffer. Directly allocate the space needed for ARGB
    unsigned int width = cinfo->output_width;
    unsigned int height = cinfo->output_height;
    bmp_buffer = (unsigned char*)malloc(width * height * 4);

    // Step 6: while (scan lines remain to be read)
    int jpg_stride = width * cinfo->output_components;
    while (cinfo->output_scanline < height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = bmp_buffer + (cinfo->output_scanline) * jpg_stride;
        jpeg_read_scanlines(cinfo, buffer_array, 1);
    }

    // Step 7: Finish decompression.
    (void)jpeg_finish_decompress(cinfo);

    // Cleanup. In this "ImageImporter" we clean up everything in "DrawToCairo"
    // as I'm not really sure whether we are able to draw a second time.
    fclose(infile);
    jpeg_destroy_decompress(cinfo);
    free(cinfo);
    cinfo = NULL;

    // --> At this point we have raw RGB data in bmp_buffer

    // Do some ugly byte shifting.
    // Byte order in libjpeg: RGB
    // Byte order in cairo and VDR: BGRA
    unsigned char temp[3];
    for (int index = (width * height) - 1; index >= 0; index--) {
        unsigned char *target = bmp_buffer + (index * 4);
        unsigned char *source = bmp_buffer + (index * 3);
        memcpy(&temp[0], source + 2, 1);
        memcpy(&temp[1], source + 1, 1);
        memcpy(&temp[2], source, 1);
        memcpy(target, &temp, 3);
    }

    // Create new Cairo surface from our raw image data
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_for_data(bmp_buffer,
                                                  CAIRO_FORMAT_RGB24,
                                                  width,
                                                  height,
                                                  width * 4);

    // Draw surface to Cairo
    if (surface) {
        cairo_set_source_surface(cr, surface, 0, 0);
        cairo_paint(cr);
        cairo_surface_destroy(surface);
    }

    // Free our memory
    free(bmp_buffer);
}

void cImageImporterJPG::GetImageSize(int &width, int &height) {
    if (cinfo) {
        width = cinfo->image_width;
        height = cinfo->image_height;
    }
}

//
// SVG Template class
//

cSVGTemplate::cSVGTemplate(string imageName, string templatePath) {
    this->imageName = imageName;
    this->templatePath = templatePath;
    startTokenColor = "{sdcol(";
    startTokenOpac = "{sdopac(";
    endToken = ")}";
    filePath = templatePath;
    filePath += imageName + ".svg";
}

cSVGTemplate::~cSVGTemplate(void) {
}

bool cSVGTemplate::Exists(void) {
    return FileExists(templatePath, imageName, "svg");
}

void cSVGTemplate::ReadTemplate(void) {
    string line;
    ifstream templatefile(filePath.c_str());
    if (templatefile.is_open()) {
        while ( getline (templatefile, line) ) {
            svgTemplate.push_back(line);
        }
        templatefile.close();
    }
}

bool cSVGTemplate::ParseTemplate(void) {
    int i = 0;
    for (vector<string>::iterator it = svgTemplate.begin(); it != svgTemplate.end(); it++) {
        string line = *it;
        size_t hit = line.find(startTokenColor);
        if (hit == string::npos) {
            i++;
            continue;
        }
        while (hit != string::npos) {
            size_t hitEnd = line.find(endToken, hit);
            if (hitEnd == string::npos) {
                esyslog("skindesigner: error in SVG Template %s: invalid tag", imageName.c_str());
                return false;
            }
            string colorName = GetColorName(line, hit, hitEnd);
            tColor replaceColor = 0x0;
            if (!config.GetThemeColor(colorName, replaceColor)) {
                esyslog("skindesigner: error in SVG Template %s: invalid color %x", imageName.c_str(), replaceColor);
                return false;
            }
            ReplaceTokens(line, hit, hitEnd, replaceColor);
            hit = line.find(startTokenColor);
        }        
        svgTemplate[i] = line;
        i++;
    }
    return true;
}

string cSVGTemplate::WriteImage(void) {
    string tempPath = *cString::sprintf("/tmp/skindesigner/svg/%s/%s/", Setup.OSDSkin, Setup.OSDTheme);
    CreateFolder(tempPath);
    string fileName = tempPath + imageName + ".svg";
    ofstream tmpimg;
    tmpimg.open (fileName.c_str(), ios::out | ios::trunc);
    if (!tmpimg.is_open()) {
        return "";
    }
    for (vector<string>::iterator it = svgTemplate.begin(); it != svgTemplate.end(); it++) {
        tmpimg << (*it) << "\n";
    }
    tmpimg.close();
    return fileName;
}

string cSVGTemplate::GetColorName(string line, size_t tokenStart, size_t tokenEnd) {
    string colorName = line.substr(tokenStart + startTokenColor.size(), tokenEnd - tokenStart - startTokenColor.size());
    if (colorName.size() > 0) {
        stringstream name;
        name << "{" << colorName << "}";
        return name.str();
    }
    return "";
}

void cSVGTemplate::ReplaceTokens(string &line, size_t tokenStart, size_t tokenEnd, tColor color) {
    string rgbColor = *cString::sprintf("%06x", color & 0x00FFFFFF);
    line.replace(tokenStart, tokenEnd - tokenStart + 2, rgbColor);
    size_t hitAlpha = line.find(startTokenOpac);
    if (hitAlpha == string::npos) {
        return;
    }
    size_t hitAlphaEnd = line.find(endToken, hitAlpha);
    if (hitAlphaEnd == string::npos) {
        return;
    }
    tIndex alpha = (color & 0xFF000000) >> 24;
    string svgAlpha = *cString::sprintf("%f", (float)(alpha / (float)255));
    std::replace( svgAlpha.begin(), svgAlpha.end(), ',', '.');
    line.replace(hitAlpha, hitAlphaEnd - hitAlpha + 2, svgAlpha);
}
