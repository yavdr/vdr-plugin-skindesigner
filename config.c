#include "config.h"
#include "libcore/helpers.h"
#include "libcore/imageloader.h"

cDesignerConfig::cDesignerConfig() {
    tmplGlobals = NULL;
    epgImagePathSet = false;
    skinPathSet = false;
    installerSkinPathSet = false;
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
    //frames per second for fading and shifting
    framesPerSecond = 40;
    //remember current skin and theme, osd size and osd fonts
    SetSkin();
    SetOSDSize();
    SetOSDFonts();
    osdLanguage = "";
    setupCloseDoReload = false;
}

cDesignerConfig::~cDesignerConfig() {
    ClearSkinSetups();
}

void cDesignerConfig::SetPathes(void) {
    if (!skinPathSet)
        skinPath = cString::sprintf("%s/skins/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    if (!installerSkinPathSet)
        installerSkinPath = cString::sprintf("%s/installerskins/", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    if (!logoPathSet) 
        logoPath = cString::sprintf("%s/logos/", cPlugin::ResourceDirectory(PLUGIN_NAME_I18N));
    if (!epgImagePathSet)
        epgImagePath = cString::sprintf("%s/epgimages/", cPlugin::CacheDirectory(PLUGIN_NAME_I18N));

    dsyslog("skindesigner: using Skin Directory %s", *skinPath);
    dsyslog("skindesigner: using Installer Skin Directory %s", *installerSkinPath);
    dsyslog("skindesigner: using common ChannelLogo Directory %s", *logoPath);
    dsyslog("skindesigner: using EPG Images Directory %s", *epgImagePath);

    vdrThemesPath = cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
    size_t found = vdrThemesPath.find("/plugins");
    if (found != string::npos)
        vdrThemesPath = vdrThemesPath.substr(0, found);
    vdrThemesPath += "/themes/";
}

void cDesignerConfig::SetSkinPath(cString path) {
    skinPath = CheckSlashAtEnd(*path);
    skinPathSet = true;
}

void cDesignerConfig::SetInstallerSkinPath(cString path) {
    installerSkinPath = CheckSlashAtEnd(*path);
    installerSkinPathSet = true;
}

void cDesignerConfig::SetLogoPath(cString path) {
    logoPath = CheckSlashAtEnd(*path);
    logoPathSet = true;
}

void cDesignerConfig::SetEpgImagePath(cString path) {
    epgImagePath = CheckSlashAtEnd(*path);
    epgImagePathSet = true;
}

void cDesignerConfig::ReadSkinFolder(cString &skinFolder, vector<string> *container) {
    DIR *folder = NULL;
    struct dirent *dirEntry;
    folder = opendir(skinFolder);
    if (!folder) {
        esyslog("skindesigner: no skins found in %s", *skinFolder);
        return;
    }
    while (dirEntry = readdir(folder)) {
        string dirEntryName = dirEntry->d_name;
        int dirEntryType = dirEntry->d_type;
        if (!dirEntryName.compare(".") || !dirEntryName.compare("..") || dirEntryType != DT_DIR)
            continue;
        container->push_back(dirEntryName);
    }
    dsyslog("skindesigner %ld skins found in %s", container->size(), *skinFolder);    
}



void cDesignerConfig::ReadSkins(void) {
    ReadSkinFolder(skinPath, &deliveredSkins);
    ReadSkinFolder(installerSkinPath, &installerSkins);
    for (vector<string>::iterator it = deliveredSkins.begin(); it != deliveredSkins.end(); it++) {
        skins.push_back(*it);
    }
    for (vector<string>::iterator it = installerSkins.begin(); it != installerSkins.end(); it++) {
        string instSkin = *it;
        bool found = false;
        for (vector<string>::iterator it2 = deliveredSkins.begin(); it2 != deliveredSkins.end(); it2++) {
            if (!instSkin.compare(*it2)) {
                found = true;
                break;
            }
        }
        if (!found)
            skins.push_back(instSkin);
    }
}

void cDesignerConfig::ClearSkinSetups(void) {
    for (map < string, cSkinSetup* >::iterator it = skinSetups.begin(); it != skinSetups.end(); it++) {
        delete it->second;
    }
    skinSetups.clear();
}

void cDesignerConfig::DebugSkinSetups(void) {
    dsyslog("skindesigner: skin setups:");
    InitSetupIterator();
    cSkinSetup *skinSetup = NULL;
    while (skinSetup = GetNextSkinSetup()) {
        skinSetup->Debug();
    }    
}

void cDesignerConfig::DebugSkinSetupParameters(void) {
    dsyslog("skindesigner: skin setup parameters:");
    for (vector<pair<string, int> >::iterator it = skinSetupParameters.begin(); it != skinSetupParameters.end(); it++) {
        dsyslog("skindesigner: parameter %s value %d", (*it).first.c_str(), (*it).second);
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

cString cDesignerConfig::GetSkinPath(string skin) {
    for (vector<string>::iterator it = deliveredSkins.begin(); it != deliveredSkins.end(); it++) {
        if (!skin.compare(*it)) {
            return skinPath;
        }
    }
    return installerSkinPath;
}

void cDesignerConfig::AddNewSkinRef(string skin) {
    cSkinDesigner *newSkin = new cSkinDesigner(skin);
    AddSkin(newSkin);
    skins.push_back(skin);
    installerSkins.push_back(skin);
    ReadSkinSetup(skin);
    map < string, cSkinSetup* >::iterator hit = skinSetups.find(skin);
    if (hit != skinSetups.end())
        (hit->second)->TranslateSetup();
}

bool cDesignerConfig::SkinActive(string skin) {
    cSkin *activeSkin = Skins.Current();
    if (activeSkin && !skin.compare(activeSkin->Name()))
        return true;
    return false;
}


void cDesignerConfig::DeleteSkin(string skin) {
    dsyslog("skindesigner: deleting skin %s", skin.c_str());
    cSkin *deleteSkin = NULL;
    bool found = false;
    for (deleteSkin = Skins.First(); deleteSkin; deleteSkin = Skins.Next(deleteSkin)) {
        if (!skin.compare(deleteSkin->Name())) {
            found = true;
            break;
        }
    }
    if (!found || !deleteSkin) {
        esyslog("skindesigner: skin %s to delete not found", skin.c_str());
        return;
    }
    vector<cSkinDesigner*>::iterator delIt;
    for (delIt = skinRefs.begin(); delIt != skinRefs.end(); delIt++) {
        if (*delIt == deleteSkin)
            break;
    }
    if (delIt != skinRefs.end())
        skinRefs.erase(delIt);

    vector<string>::iterator delIt2; 
    for (delIt2 = installerSkins.begin(); delIt2 != installerSkins.end(); delIt2++) {
        if (!skin.compare(*delIt2))
            break;
    }
    if (delIt2 != installerSkins.end())
        installerSkins.erase(delIt2);

    vector<string>::iterator delIt3; 
    for (delIt3 = skins.begin(); delIt3 != skins.end(); delIt3++) {
        if (!skin.compare(*delIt3))
            break;
    }
    if (delIt3 != skins.end())
        skins.erase(delIt3);

    Skins.Del(deleteSkin);

    stringstream deletePath;
    deletePath << *installerSkinPath << skin;
    string delPath = deletePath.str();
    if (FolderExists(delPath)) {
        dsyslog("skindesigner: deleting skin %s", delPath.c_str());
        cString command = cString::sprintf("rm -rf %s", delPath.c_str());
        system(*command); 
    }
}

cSkinDesigner* cDesignerConfig::GetNextSkinRef(void) {
    if (skinRefsIterator == skinRefs.end()) {
        return NULL;
    }
    cSkinDesigner *skin = *skinRefsIterator;
    skinRefsIterator++;
    return skin;    
}

cSkinSetup* cDesignerConfig::GetSkinSetup(string &skin) {
    map< string, cSkinSetup* >::iterator hit = skinSetups.find(skin);
    if (hit != skinSetups.end()) {
        return hit->second;
    }
    return NULL;
}

cSkinSetup* cDesignerConfig::GetNextSkinSetup(void) {
    if (setupIt == skinSetups.end()) {
        return NULL;
    }
    cSkinSetup* skinSetup = setupIt->second;
    setupIt++;
    return skinSetup;
}

cSkinSetupMenu* cDesignerConfig::GetSkinSetupMenu(string &skin, string &menu) {
    cSkinSetup *skinSetup = GetSkinSetup(skin);
    if (!skinSetup)
        return NULL;
    return skinSetup->GetMenu(menu);
}


void cDesignerConfig::TranslateSetup(void) {
    for (map< string, cSkinSetup* >::iterator it = skinSetups.begin(); it != skinSetups.end(); it++) {
        (it->second)->TranslateSetup();
    }
}

void cDesignerConfig::UpdateSkinSetupParameter(string name, int value) {
    vector<pair<string,int> >::iterator hit;
    for (hit = skinSetupParameters.begin(); hit != skinSetupParameters.end(); hit++) {
        if (!name.compare((*hit).first)) {
            skinSetupParameters.erase(hit);
            break;
        }
    }
    skinSetupParameters.push_back(pair<string,int>(name, value));
}

void cDesignerConfig::SetSkinSetupParameters(void) {
    for (vector < pair <string, int> >::iterator it = skinSetupParameters.begin(); it != skinSetupParameters.end(); it++) {
        string name = (*it).first;
        int value = (*it).second;

        InitSkinIterator();
        string activeSkin = "";
        bool skinFound = false;
        while (GetSkin(activeSkin)) {
            stringstream checkString;
            checkString << activeSkin << ".";
            size_t hit = name.find(checkString.str());
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

void cDesignerConfig::ReadSkinRepos(void) {
    skinRepos.Read(*skinPath);
    dsyslog("skindesigner: read %d skinrepositories from %s", skinRepos.Count(), *skinPath);
}

bool cDesignerConfig::SkinInstalled(string name) {
    for (vector<string>::iterator it = installerSkins.begin(); it != installerSkins.end(); it++) {
        if (!name.compare(*it))
            return true;
    }
    return false;
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

void cDesignerConfig::AddPluginMenus(string name, map< int, string > menus) {
    pluginMenus.insert(pair< string, map < int, string > >(name, menus));
}

void cDesignerConfig::AddPluginViews(string name, 
                                     map< int, string > views,
                                     multimap< int, pair <int, string> > subViews,
                                     map< int, map <int, string> > viewElements,
                                     map< int, map <int, string> > viewGrids) {
    pluginViews.insert(pair< string, map < int, string > >(name, views));
    pluginSubViews.insert(pair< string, multimap< int, pair <int, string> > >(name, subViews));
    pluginViewElements.insert(pair< string, map< int, map <int, string> > >(name, viewElements));
    pluginViewGrids.insert(pair< string, map< int, map <int, string> > >(name, viewGrids));
}

void cDesignerConfig::InitPluginMenuIterator(void) {
    plugMenuIt = pluginMenus.begin();
}

map <int,string> *cDesignerConfig::GetPluginTemplates(string &name) {
    if (plugMenuIt == pluginMenus.end())
        return NULL;
    name = plugMenuIt->first;
    map <int,string> *templates = &plugMenuIt->second;
    plugMenuIt++;
    return templates; 
}

void cDesignerConfig::InitPluginViewIterator(void) {
    plugViewIt = pluginViews.begin();
}

map <int,string> *cDesignerConfig::GetPluginViews(string &name) {
    if (plugViewIt == pluginViews.end())
        return NULL;
    name = plugViewIt->first;
    map <int,string> *views = &plugViewIt->second;
    plugViewIt++;
    return views; 
}

map <int,string> cDesignerConfig::GetPluginSubViews(string name, int viewID) {
    map <int,string> subViews;

    map < string, multimap< int, pair <int, string> > >::iterator hit = pluginSubViews.find(name);
    if (hit == pluginSubViews.end())
        return subViews;

    multimap< int, pair<int, string> > subs = hit->second;

    pair < multimap< int, pair<int, string> >::iterator, multimap< int, pair<int, string> >::iterator> viewSubViews;
    viewSubViews = subs.equal_range(viewID); 

    for (multimap< int, pair<int, string> >::iterator it=viewSubViews.first; it!=viewSubViews.second; ++it) {
        pair<int, string> subViewFound = it->second;
        subViews.insert(pair<int,string>(subViewFound.first, subViewFound.second));
    }
    return subViews;
}

int cDesignerConfig::GetPluginViewElementID(string pluginName, string viewElementName, int viewID) {
    map < string, map< int, map <int, string> > >::iterator hit = pluginViewElements.find(pluginName);
    if (hit == pluginViewElements.end())
        return -1;
    map< int, map <int, string> >::iterator hit2 = (hit->second).find(viewID);
    if (hit2 == (hit->second).end())
        return -1;
    
    map <int, string> viewElements = hit2->second;
    for (map <int, string>::iterator it = viewElements.begin(); it != viewElements.end(); it++) {
        if (!(it->second).compare(viewElementName))
            return it->first;
    }
    return -1;
}

int cDesignerConfig::GetPluginViewGridID(string pluginName, string viewGridName, int viewID) {
    map < string, map< int, map <int, string> > >::iterator hit = pluginViewGrids.find(pluginName);
    if (hit == pluginViewGrids.end())
        return -1;
    map< int, map <int, string> >::iterator hit2 = (hit->second).find(viewID);
    if (hit2 == (hit->second).end())
        return -1;
    
    map <int, string> viewGrids = hit2->second;
    for (map <int, string>::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
        if (!(it->second).compare(viewGridName))
            return it->first;
    }
    return -1;
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
    else if (!strcasecmp(Name, "FramesPerSecond"))         framesPerSecond = atoi(Value);
    else pluginSetupParam = false;

    if (!pluginSetupParam) {
        skinSetupParameters.push_back(pair <string, int>(Name, atoi(Value)));
    }

    return true;
}
