#include "config.h"
#include "libcore/helpers.h"
#include "libcore/imageloader.h"

cDesignerConfig::cDesignerConfig() {
    tmplGlobals = NULL;
    epgImagePathSet = false;
    skinPathSet = false;
    logoPathSet = false;
    //Common
    numLogosPerSizeInitial = 30;
    limitLogoCache = 1;
    numLogosMax = 200;
    debugImageLoading = 0;
    replaceDecPoint = false;
    //settings for rerun display
    rerunAmount = 10;
    rerunDistance = 2;
    rerunMaxChannel = 0;
    //menu display style, display menu items 
    //one after each other or in one step
    blockFlush = 1;
    //remember current skin and theme, osd size and osd fonts
    SetSkin();
    SetOSDSize();
    SetOSDFonts();
    osdLanguage = Setup.OSDLanguage;
}

cDesignerConfig::~cDesignerConfig() {
    ClearSkinSetups();
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
    dsyslog("skindesigner %ld skins found in %s", skins.size(), *skinPath);
}

void cDesignerConfig::ClearSkinSetups(void) {
    for (map < string, cSkinSetup* >::iterator it = skinSetups.begin(); it != skinSetups.end(); it++) {
        delete it->second;
    }
}

void cDesignerConfig::ReadSkinSetup(string skin) {
    cSkinSetup *skinSetup = new cSkinSetup(skin);
    if (skinSetup->ReadFromXML()) {
        //skinSetup->Debug();
        skinSetups.insert(pair<string, cSkinSetup* >(skin, skinSetup));
    }
}

bool cDesignerConfig::GetSkin(string &skin) {
    if (skinIterator == skins.end()) {
        return false;
    }
    skin = *skinIterator;
    skinIterator++;
    return true;
}

cSkinSetup* cDesignerConfig::GetSkinSetup(string &skin) {
    map< string, cSkinSetup* >::iterator hit = skinSetups.find(skin);
    if (hit != skinSetups.end()) {
        return hit->second;
    }
    return NULL;
}

cSkinSetup* cDesignerConfig::GetSkinSetup(void) {
    if (setupIt == skinSetups.end()) {
        return NULL;
    }
    cSkinSetup* skinSetup = setupIt->second;
    setupIt++;
    return skinSetup;
}

void cDesignerConfig::TranslateSetup(void) {
    for (map< string, cSkinSetup* >::iterator it = skinSetups.begin(); it != skinSetups.end(); it++) {
        (it->second)->TranslateSetup();
    }
}

void cDesignerConfig::CheckUnknownSetupParameters(void) {
    for (vector < pair <string, int> >::iterator it = unknownSetupParameters.begin(); it != unknownSetupParameters.end(); it++) {
        string name = (*it).first;
        int value = (*it).second;

        InitSkinIterator();
        string activeSkin = "";
        bool skinFound = false;
        while (GetSkin(activeSkin)) {
            size_t hit = name.find(activeSkin);
            if (hit != 0)
                continue;
            skinFound = true;
            break;
        }
        if (skinFound) {
            cSkinSetup* skinSetup = GetSkinSetup(activeSkin);
            if (skinSetup != NULL) {
                string paramName = name.substr(activeSkin.size()+1);
                cSkinSetupParameter *skinSetupParam = skinSetup->GetParameter(paramName);
                if (skinSetupParam) {
                    skinSetupParam->value = value;
                    continue;
                }
            }
        }
        esyslog("skindesigner: ERROR Unknown Setup Parameter %s", name.c_str());
    }
}

void cDesignerConfig::UpdateGlobals(void) {
    string activeSkin = Setup.OSDSkin;
    cSkinSetup *skinSetupActiveSkin = GetSkinSetup(activeSkin);
    if (skinSetupActiveSkin && tmplGlobals) {
        dsyslog("skindesigner: globals for skin %s adapted to skin setup", activeSkin.c_str());
        skinSetupActiveSkin->AddToGlobals(tmplGlobals);
    }
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

void cDesignerConfig::SetSkin(void) {
    osdSkin =  Setup.OSDSkin;
    osdTheme = Setup.OSDTheme;
}

bool cDesignerConfig::SkinChanged(void) {
    bool changed = false;
    if (osdSkin.compare(Setup.OSDSkin) != 0) {
        dsyslog("skindesigner: skin changed from %s to %s", osdSkin.c_str(), Setup.OSDSkin);
        changed = true;
    }
    if (osdTheme.compare(Setup.OSDTheme) != 0) {
        dsyslog("skindesigner: theme changed from %s to %s", osdTheme.c_str(), Setup.OSDTheme);
        changed = true;
    }
    if (changed)
        SetSkin();
    return changed;
}

void cDesignerConfig::SetOSDSize(void) {
    osdSize.SetWidth(cOsd::OsdWidth());
    osdSize.SetHeight(cOsd::OsdHeight());
    osdSize.SetX(cOsd::OsdLeft());
    osdSize.SetY(cOsd::OsdTop());
}

bool cDesignerConfig::OsdSizeChanged(void) {
   if ((osdSize.Width() != cOsd::OsdWidth()) ||
        (osdSize.Height() != cOsd::OsdHeight()) ||
        (osdSize.X() != cOsd::OsdLeft()) ||
        (osdSize.Y() != cOsd::OsdTop())) {
        dsyslog("skindesigner: osd size changed");
        dsyslog("skindesigner: old osd size: top %d left %d size %d * %d", osdSize.X(), osdSize.Y(), osdSize.Width(), osdSize.Height());
        SetOSDSize();
        dsyslog("skindesigner: new osd size: top %d left %d size %d * %d", osdSize.X(), osdSize.Y(), osdSize.Width(), osdSize.Height());
        return true;
    }
    return false; 
}

void cDesignerConfig::SetOSDFonts(void) {
    fontFix = Setup.FontFix;
    fontOsd = Setup.FontOsd;
    fontSml = Setup.FontSml;
}

bool cDesignerConfig::OsdFontsChanged(void) {
    bool changed = false;
    if (fontFix.compare(Setup.FontFix) != 0) {
        changed = true;
    }
    if (fontOsd.compare(Setup.FontOsd) != 0) {
        changed = true;
    }
    if (fontSml.compare(Setup.FontSml) != 0) {
        changed = true;
    }
    if (changed)
        SetOSDFonts();
    return changed;
}

bool cDesignerConfig::OsdLanguageChanged(void) {
    if (osdLanguage.compare(Setup.OSDLanguage)) {
        osdLanguage = Setup.OSDLanguage;
        return true;
    }
    return false;
}

cString cDesignerConfig::GetSkinRessourcePath(void) {
    return cString::sprintf("%s%s", *skinPath, osdSkin.c_str());
}

void cDesignerConfig::AddPlugin(string name, map < int, string > &menus) {
    plugins.insert(pair< string, map < int, string > >(name, menus));
}

void cDesignerConfig::InitPluginIterator(void) {
    plugIt = plugins.begin();
}

map <int,string> *cDesignerConfig::GetPluginTemplates(string &name) {
    if (plugIt == plugins.end())
        return NULL;
    name = plugIt->first;
    map <int,string> *templates = &plugIt->second;
    plugIt++;
    return templates; 
}

cString cDesignerConfig::CheckSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}

bool cDesignerConfig::SetupParse(const char *Name, const char *Value) {
    bool pluginSetupParam = true;
    if      (!strcasecmp(Name, "DebugImageLoading"))       debugImageLoading = atoi(Value);
    else if (!strcasecmp(Name, "LimitChannelLogoCache"))   limitLogoCache = atoi(Value);
    else if (!strcasecmp(Name, "NumberLogosInitially"))    numLogosPerSizeInitial = atoi(Value);
    else if (!strcasecmp(Name, "NumberLogosMax"))          numLogosMax = atoi(Value);
    else if (!strcasecmp(Name, "RerunAmount"))             rerunAmount = atoi(Value);
    else if (!strcasecmp(Name, "RerunDistance"))           rerunDistance = atoi(Value);
    else if (!strcasecmp(Name, "RerunMaxChannel"))         rerunMaxChannel = atoi(Value);
    else if (!strcasecmp(Name, "BlockFlush"))              blockFlush = atoi(Value);
    else pluginSetupParam = false;

    if (!pluginSetupParam) {
        unknownSetupParameters.push_back(pair <string, int>(Name, atoi(Value)));
    }

    return true;
}
