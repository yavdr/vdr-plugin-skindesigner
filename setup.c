#include "setup.h"

cSkinDesignerSetup::cSkinDesignerSetup() {
    data = config;
    Setup();
}

cSkinDesignerSetup::~cSkinDesignerSetup() {
}


void cSkinDesignerSetup::Setup(void) {
    int current = Current();
    Clear();

    Add(new cMenuEditBoolItem(tr("Debug Image Loading"), &data.debugImageLoading));

    Add(new cMenuEditBoolItem(tr("Limit Channel Logo Cache"), &data.limitLogoCache));
    Add(new cMenuEditIntItem(tr("Number to cache initially (per size)"), &data.numLogosPerSizeInitial, 0, 1000));
    Add(new cMenuEditIntItem(tr("Number to cache in maximum"), &data.numLogosMax, 0, 1000));

    cString message = cString::sprintf("--------------------- %s ---------------------", tr("Cache Statistics"));
    Add(new cOsdItem(*message));
    cList<cOsdItem>::Last()->SetSelectable(false);

    int sizeIconCache = 0;
    int numIcons = 0;
    imgCache->GetIconCacheSize(numIcons, sizeIconCache);
    cString iconCacheInfo = cString::sprintf("%s %d %s - %s %d %s", tr("cached"), numIcons, tr("icons"), tr("size"), sizeIconCache, tr("byte"));
    Add(new cOsdItem(*iconCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);
    
    int sizeLogoCache = 0;
    int numLogos = 0;
    imgCache->GetLogoCacheSize(numLogos, sizeLogoCache);
    cString logoCacheInfo = cString::sprintf("%s %d %s - %s %d %s", tr("cached"), numLogos, tr("logos"), tr("size"), sizeLogoCache, tr("byte"));
    Add(new cOsdItem(*logoCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);

    int sizeSkinpartCache = 0;
    int numSkinparts = 0;
    imgCache->GetSkinpartsCacheSize(numSkinparts, sizeSkinpartCache);
    cString skinpartCacheInfo = cString::sprintf("%s %d %s - %s %d %s", tr("cached"), numSkinparts, tr("skinparts"), tr("size"), sizeSkinpartCache, tr("byte"));
    Add(new cOsdItem(*skinpartCacheInfo));
    cList<cOsdItem>::Last()->SetSelectable(false);

    SetCurrent(Get(current));
    Display();
}

eOSState cSkinDesignerSetup::ProcessKey(eKeys Key) {
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    switch (state) {
        case osContinue: {
            if (NORMALKEY(Key) == kUp || NORMALKEY(Key) == kDown) {
                cOsdItem* item = Get(Current());
                if (item)
                    item->ProcessKey(kNone);
            }
            break; }
        default: break;
    }
    return state;
}

void cSkinDesignerSetup::Store(void) {
    config = data;

    SetupStore("DebugImageLoading", config.debugImageLoading);
    SetupStore("LimitChannelLogoCache", config.limitLogoCache);
    SetupStore("NumberLogosInitially", config.numLogosPerSizeInitial);
    SetupStore("NumberLogosMax", config.numLogosMax);
}