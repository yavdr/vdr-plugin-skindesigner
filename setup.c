#include "setup.h"

cSkinDesignerSetup::cSkinDesignerSetup() {
    numLogosPerSizeInitial = config.numLogosPerSizeInitial;
    limitLogoCache = config.limitLogoCache;
    numLogosMax = config.numLogosMax;
    debugImageLoading = config.debugImageLoading;
    rerunAmount = config.rerunAmount;
    rerunDistance = config.rerunDistance;
    rerunMaxChannel = config.rerunMaxChannel;
    blockFlush = config.blockFlush;
    menuDisplayStyle[0] = tr("after one another");
    menuDisplayStyle[1] = tr("at one go");
    Setup();
}

cSkinDesignerSetup::~cSkinDesignerSetup() {
}


void cSkinDesignerSetup::Setup(void) {
    int current = Current();
    Clear();

    SkinSetup();
    PluginSetup();
    ImageCacheStatistics();

    SetCurrent(Get(current));
    Display();
}

eOSState cSkinDesignerSetup::ProcessKey(eKeys Key) {
    bool hadSubMenu = HasSubMenu();
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    if (hadSubMenu && Key == kOk) {
        esyslog("skindesigner: store submenu");
    }
    if (!hadSubMenu && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu)) {
            switch (Key) {
                case kOk: {
                    string itemText = Get(Current())->Text();
                    size_t hit = itemText.find(tr("Skin"));
                    if (hit == 0) {
                        string skin = itemText.substr(strlen(tr("Skin"))+1);
                        state = AddSubMenu(new cSkindesignerSkinSetup(skin));
                    }
                    break;
                } default:
                    break;
            }
        }
    }
    return state;
}

void cSkinDesignerSetup::Store(void) {
    config.numLogosPerSizeInitial = numLogosPerSizeInitial;
    config.limitLogoCache = limitLogoCache;
    config.numLogosMax = numLogosMax;
    config.debugImageLoading = debugImageLoading;
    config.rerunAmount = rerunAmount;
    config.rerunDistance = rerunDistance;
    config.rerunMaxChannel = rerunMaxChannel;
    config.blockFlush = blockFlush;

    config.InitSetupIterator();
    cSkinSetup *skinSetup = NULL;
    while (skinSetup = config.GetSkinSetup()) {
        string skin = skinSetup->GetSkin();
        skinSetup->InitParameterIterator();
        cSkinSetupParameter *param = NULL;
        while (param = skinSetup->GetParameter()) {
            SetupStore(*cString::sprintf("%s.%s", skin.c_str(), param->name.c_str()), param->value);
        }
    }
    config.UpdateGlobals();

    SetupStore("DebugImageLoading", debugImageLoading);
    SetupStore("LimitChannelLogoCache", limitLogoCache);
    SetupStore("NumberLogosInitially", numLogosPerSizeInitial);
    SetupStore("NumberLogosMax", numLogosMax);
    SetupStore("RerunAmount", rerunAmount);
    SetupStore("RerunDistance", rerunDistance);
    SetupStore("RerunMaxChannel", rerunMaxChannel);
    SetupStore("BlockFlush", blockFlush);
}

cOsdItem *cSkinDesignerSetup::InfoItem(const char *label) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("---------------- %s ----------------", tr(label)));
    item->SetSelectable(false);
    return item;
}

void cSkinDesignerSetup::PluginSetup(void) {
    Add(InfoItem("Plugin Setup"));

    Add(new cMenuEditStraItem(tr("Menu Item display method"), &blockFlush, 2, menuDisplayStyle));

    Add(InfoItem("Reruns"));
    Add(new cMenuEditIntItem(tr("Maximum number of reruns to display"), &rerunAmount, 1, 100));
    Add(new cMenuEditIntItem(tr("Minimum timely distance of rerun (in hours)"), &rerunDistance, 0, 1000));
    Add(new cMenuEditIntItem(tr("Limit Channel Numbers (0 = no limit)"), &rerunMaxChannel, 0, 1000));

    Add(InfoItem("Image Loading"));
    Add(new cMenuEditBoolItem(tr("Debug Image Loading"), &debugImageLoading));
    Add(new cMenuEditBoolItem(tr("Limit Channel Logo Cache"), &limitLogoCache));
    Add(new cMenuEditIntItem(tr("Number to cache initially (per size)"), &numLogosPerSizeInitial, 0, 1000));
    Add(new cMenuEditIntItem(tr("Number to cache in maximum"), &numLogosMax, 0, 1000));
}

void cSkinDesignerSetup::SkinSetup(void) {
    Add(InfoItem("Skin Setup"));

    config.InitSkinIterator();
    string skin = "";
    while (config.GetSkin(skin)) {
        cSkinSetup *skinSetup = config.GetSkinSetup(skin);
        if (!skinSetup) {
            Add(new cOsdItem(cString::sprintf("%s %s %s", tr("Skin"), skin.c_str(), tr("has no setup"))));
            cList<cOsdItem>::Last()->SetSelectable(false);
        } else {
            Add(new cOsdItem(cString::sprintf("%s %s", tr("Skin"), skin.c_str())));
        }
    }
}

void cSkinDesignerSetup::ImageCacheStatistics(void) {
    if (!imgCache) {
        return;
    }

    Add(InfoItem("Cache Statistics"));
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
}

// --- cSkindesignerSkinSetup -----------------------------------------------------------

cSkindesignerSkinSetup::cSkindesignerSkinSetup(string skin)  : cOsdMenu(*cString::sprintf("%s: %s \"%s\"", trVDR("Setup"), tr("Skin"), skin.c_str()), 30) {
    this->skin = skin;
    Set();
}

cSkindesignerSkinSetup::~cSkindesignerSkinSetup() {
}

eOSState cSkindesignerSkinSetup::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
            case kOk:
                return osBack;
            default:
                break;
        }
    }
    return state;
}

void cSkindesignerSkinSetup::Set(void) {
    cSkinSetup *skinSetup = config.GetSkinSetup(skin);
    if (!skinSetup)
        return;

    skinSetup->InitParameterIterator();
    cSkinSetupParameter *param = NULL;
    while (param = skinSetup->GetParameter()) {
        if (param->type == sptInt) {
            Add(new cMenuEditIntItem(param->displayText.c_str(), &param->value, param->min, param->max));
        } else if (param->type == sptBool) {
            Add(new cMenuEditBoolItem(param->displayText.c_str(), &param->value));
        }
    }

}