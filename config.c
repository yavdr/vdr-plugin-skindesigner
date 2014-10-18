#include "config.h"
#include "libcore/helpers.h"
#include "libcore/imageloader.h"

cDesignerConfig::cDesignerConfig() {
    epgImagePathSet = false;
    skinPathSet = false;
    logoPathSet = false;
    //Common
    logoExtension = "png";
    numLogosPerSizeInitial = 30;
    limitLogoCache = 1;
    numLogosMax = 200;
    debugImageLoading = 0;
    //default logo width and height
    logoWidth = 268;
    logoHeight = 200;
    replaceDecPoint = false;
    //settings for rerun display
    rerunAmount = 10;
    rerunDistance = 2;
    rerunMaxChannel = 0;
    //menu display style, display menu items 
    //one after each other or in one step
    blockFlush = 1;
}

cDesignerConfig::~cDesignerConfig() {
}

void cDesignerConfig::SetPathes(void) {
    if (!skinPathSet)
        skinPath = cString::sprintf("%s/skins/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    if (!logoPathSet) 
        logoPath = cString::sprintf("%s/logos/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    if (!epgImagePathSet)
        epgImagePath = cString::sprintf("%s/epgimages/", cPlugin::CacheDirectory(PLUGIN_NAME_I18N));

    dsyslog("skindesigner: using Skin Directory %s", *skinPath);
    dsyslog("skindesigner: using common ChannelLogo Directory %s", *logoPath);
    dsyslog("skindesigner: using EPG Images Directory %s", *epgImagePath);
}

void cDesignerConfig::SetSkinPath(cString path) {
    skinPath = CheckSlashAtEnd(*path);
    skinPathSet = true;
}

void cDesignerConfig::SetLogoPath(cString path) {
    logoPath = CheckSlashAtEnd(*path);
    logoPathSet = true;
}

void cDesignerConfig::SetEpgImagePath(cString path) {
    epgImagePath = CheckSlashAtEnd(*path);
    epgImagePathSet = true;
}

void cDesignerConfig::ReadSkins(void) {
    DIR *folder = NULL;
    struct dirent *dirEntry;
    folder = opendir(skinPath);
    if (!folder) {
        esyslog("skindesigner: no skins found in %s", *skinPath);
        return;
    }
    while (dirEntry = readdir(folder)) {
        string dirEntryName = dirEntry->d_name;
        int dirEntryType = dirEntry->d_type;
        if (!dirEntryName.compare(".") || !dirEntryName.compare("..") || dirEntryType != DT_DIR)
            continue;
        skins.push_back(dirEntryName);
    }
    dsyslog("skindesigner %d skins found in %s", skins.size(), *skinPath);
}

bool cDesignerConfig::GetSkin(string &skin) {
    if (skinIterator == skins.end()) {
        return false;
    }
    skin = *skinIterator;
    skinIterator++;
    return true;
}


void cDesignerConfig::SetChannelLogoSize(void) {
    cImageLoader imgLoader;
    imgLoader.DeterminateChannelLogoSize(logoWidth, logoHeight);
    dsyslog("skindesigner: using %dx%d px as original channel logo size", logoWidth, logoHeight);
}

void cDesignerConfig::CheckDecimalPoint(void) {
    struct lconv *pLocInfo;
    pLocInfo = localeconv();
    string decimalPoint = pLocInfo->decimal_point;
    if (decimalPoint.compare(".")) {
        dsyslog("skindesigner: using decimal point %s", decimalPoint.c_str());
        replaceDecPoint = true;
        decPoint = decimalPoint[0];
    }
}

cString cDesignerConfig::CheckSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}

bool cDesignerConfig::SetupParse(const char *Name, const char *Value) {
    if      (!strcasecmp(Name, "DebugImageLoading"))       debugImageLoading = atoi(Value);
    else if (!strcasecmp(Name, "LimitChannelLogoCache"))   limitLogoCache = atoi(Value);
    else if (!strcasecmp(Name, "NumberLogosInitially"))    numLogosPerSizeInitial = atoi(Value);
    else if (!strcasecmp(Name, "NumberLogosMax"))          numLogosMax = atoi(Value);
    else if (!strcasecmp(Name, "RerunAmount"))             rerunAmount = atoi(Value);
    else if (!strcasecmp(Name, "RerunDistance"))           rerunDistance = atoi(Value);
    else if (!strcasecmp(Name, "RerunMaxChannel"))         rerunMaxChannel = atoi(Value);
    else if (!strcasecmp(Name, "BlockFlush"))              blockFlush = atoi(Value);
    else return false;
    return true;
}
