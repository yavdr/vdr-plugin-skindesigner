#ifndef __SKINDESIGNER_SETUP_H
#define __SKINDESIGNER_SETUP_H

#include "config.h"

// --- cSkinDesignerSetup -----------------------------------------------------------

class cSkinDesignerSetup : public cMenuSetupPage {
private:
    int numLogosPerSizeInitial;
    int limitLogoCache;
    int numLogosMax;
    int debugImageLoading;
    int rerunAmount;
    int rerunDistance;
    int rerunMaxChannel;
    int blockFlush;
    const char *menuDisplayStyle[2];

    void Setup(void);
    virtual void Store(void);
    virtual eOSState ProcessKey(eKeys Key);
    cOsdItem *InfoItem(const char *label);
    void PluginSetup(void);
    void SkinSetup(void);
    void ImageCacheStatistics(void);
public:
    cSkinDesignerSetup(void);
    virtual ~cSkinDesignerSetup();
};

// --- cSkindesignerSkinSetup -----------------------------------------------------------

class cSkindesignerSkinSetup : public cOsdMenu {
private:
    string skin;
protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
public:
    cSkindesignerSkinSetup(string skin);
    virtual ~cSkindesignerSkinSetup();
};

#endif //__SKINDESIGNER_SETUP_H