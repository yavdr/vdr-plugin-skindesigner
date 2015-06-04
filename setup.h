#ifndef __SKINDESIGNER_SETUP_H
#define __SKINDESIGNER_SETUP_H

#include "config.h"
#include "libskindesignerapi/skindesignerosdbase.h"

enum eItemType {
    itSkinSetup,
    itNoSkinSetup,
    itSkinRepo
};

// --- cInstallManager -----------------------------------------------------------
class cInstallManager {
private:
    uint64_t installationStart;
    int lastInstallDuration;
    int timeout;
    bool installing;
    bool updating;
    cSkinRepo *runningInst;
protected:
    string currentSkin;
    bool StartInstallation(string skin);
    bool StartUpdate(string skin);
    bool Installing(void) { return installing; };
    bool Updating(void) { return updating; };
    bool Finished(void);
    bool SuccessfullyInstalled(void);
    bool SuccessfullyUpdated(void);
    int Duration(void);
    eOSState ProcessInstallationStatus(void);
public:
    cInstallManager(void);
    virtual ~cInstallManager(void);
};

// --- cSkinDesignerSetup -----------------------------------------------------------
class cSkinDesignerSetup : public cMenuSetupPage, cInstallManager {
private:
    int numLogosPerSizeInitial;
    int limitLogoCache;
    int numLogosMax;
    int debugImageLoading;
    int rerunAmount;
    int rerunDistance;
    int rerunMaxChannel;
    int blockFlush;
    int framesPerSecond;
    const char *menuDisplayStyle[2];
    void Setup(void);
    virtual void Store(void);
    virtual eOSState ProcessKey(eKeys Key);
    cOsdItem *InfoItem(const char *label);
    void PluginSetup(void);
    void ImageCacheStatistics(void);
    void SkinSetup(void);
    void InstallSkins(void);
public:
    cSkinDesignerSetup(void);
    virtual ~cSkinDesignerSetup(void);
};

// --- cSkinMenuItem -----------------------------------------------------------

class cSkinMenuItem : public cOsdItem {
private:
    string skinName;
    eItemType type;
public:
    cSkinMenuItem(string skinName, string displayText, eItemType type);
    virtual ~cSkinMenuItem() {};
    string GetSkinName(void) { return skinName; };
    eItemType Type(void) { return type; };
};

// --- cSkinSetupSubMenu -----------------------------------------------------------

class cSkinSetupSubMenu : public cOsdItem {
private:
    string name;
public:
    cSkinSetupSubMenu(string name, string displayText);
    virtual ~cSkinSetupSubMenu() {};
    string GetName(void) { return name; };
};

// --- cSkindesignerSkinSetup -----------------------------------------------------------

class cSkindesignerSkinSetup : public cOsdMenu, cInstallManager {
private:
    string skin;
    string name;
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cSkindesignerSkinSetup(string skin, string menu);
    virtual ~cSkindesignerSkinSetup();
};

// --- cSkindesignerSkinPreview -----------------------------------------------------------

class cSkindesignerSkinPreview : public skindesignerapi::cSkindesignerOsdMenu, cInstallManager {
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    string CheckFontInstalled(string fontName);
public:
    cSkindesignerSkinPreview(string skin);
    virtual ~cSkindesignerSkinPreview();
    void Display(void);
};

#endif //__SKINDESIGNER_SETUP_H