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
#include "libcore/recfolderinfo.h"
#include "libcore/skinsetup.h"

#define SCRIPTOUTPUTPATH "/tmp/skindesigner"

class cDesignerConfig {
private:
    cString CheckSlashAtEnd(string path);
    bool epgImagePathSet;
    bool skinPathSet;
    bool logoPathSet;
    cRect osdSize;
    string osdSkin;
    string osdTheme;
    string fontFix;
    string fontOsd;
    string fontSml;
    string osdLanguage;
    cGlobals *tmplGlobals;
    map < string, map < int, string > > plugins;
    map < string, map < int, string > >::iterator plugIt;
    map < string, cSkinSetup* > skinSetups;
    map < string, cSkinSetup* >::iterator setupIt;
    vector < pair <string, int> > unknownSetupParameters;
public:
    cDesignerConfig();
    ~cDesignerConfig();
    bool SetupParse(const char *Name, const char *Value);
    void SetPathes(void);
    void SetSkinPath(cString path);
    void SetLogoPath(cString path);
    void SetEpgImagePath(cString path);
    void ReadSkins(void);
    void ReadSkinSetup(string skin);
    void InitSkinIterator(void) { skinIterator = skins.begin(); };
    bool GetSkin(string &skin);
    void ClearSkinSetups(void);
    cSkinSetup* GetSkinSetup(string &skin);
    cSkinSetup* GetSkinSetup(void);
    void InitSetupIterator(void) { setupIt = skinSetups.begin(); };
    void TranslateSetup(void);
    void CheckUnknownSetupParameters(void);
    void SetGlobals(cGlobals *globals) { tmplGlobals = globals; };
    void UpdateGlobals(void);
    void CheckDecimalPoint(void);
    void SetSkin(void);
    bool SkinChanged(void);
    void SetOSDSize(void);
    bool OsdSizeChanged(void);
    void SetOSDFonts(void);
    bool OsdFontsChanged(void);
    bool OsdLanguageChanged(void);
    cString GetSkinRessourcePath(void);
    void AddPlugin(string name, map < int, string > &menus);
    void InitPluginIterator(void);
    map <int,string> *GetPluginTemplates(string &name);
    cString skinPath;
    cString logoPath;
    cString epgImagePath;
    vector<string> skins;
    vector<string>::iterator skinIterator;
    bool replaceDecPoint;
    char decPoint;
    //Setup Parameter
    int numLogosPerSizeInitial;
    int limitLogoCache;
    int numLogosMax;
    int debugImageLoading;
    int rerunAmount;
    int rerunDistance;
    int rerunMaxChannel;
    int blockFlush;
};

#ifdef DEFINE_CONFIG
    bool firstDisplay = true;
    cDesignerConfig config;
    cFontManager *fontManager = NULL;
    cImageCache *imgCache = NULL;
    cTheme Theme;
    cRecordingsFolderInfo recFolderInfo(Recordings);
#else
    extern bool firstDisplay;
    extern cDesignerConfig config;
    extern cFontManager *fontManager;
    extern cImageCache *imgCache;
    extern cTheme Theme;
    extern cRecordingsFolderInfo recFolderInfo;
#endif

#endif //__DESIGNER_CONFIG_H
