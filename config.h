#ifndef __DESIGNER_CONFIG_H
#define __DESIGNER_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <vdr/tools.h>
#include <vdr/skins.h>
#include <vdr/plugin.h>
#include "libcore/fontmanager.h"
#include "libcore/imagecache.h"

class cDesignerConfig {
private:
    cString CheckSlashAtEnd(std::string path);
    bool epgImagePathSet;
    bool skinPathSet;
public:
    cDesignerConfig();
    ~cDesignerConfig();
    bool SetupParse(const char *Name, const char *Value);
    void SetSkinPath(cString path);
    void SetEpgImagePath(cString path);
    void SetPathes(void);
    void SetChannelLogoSize(void);
    void CheckDecimalPoint(void);
    cString logoExtension;
    cString skinPath;
    cString epgImagePath;
    int numLogosPerSizeInitial;
    int limitLogoCache;
    int numLogosMax;
    int debugImageLoading;
    int logoWidth;
    int logoHeight;
    bool replaceDecPoint;
    char decPoint;
};
#ifdef DEFINE_CONFIG
    bool firstDisplay = true;
    cDesignerConfig config;
    cFontManager *fontManager;
    cImageCache *imgCache;
    cTheme Theme;
#else
    extern bool firstDisplay;
    extern cDesignerConfig config;
    extern cFontManager *fontManager;
    extern cImageCache *imgCache;
    extern cTheme Theme;
#endif

#endif //__DESIGNER_CONFIG_H
