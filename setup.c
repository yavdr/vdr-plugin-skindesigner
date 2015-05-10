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
    framesPerSecond = config.framesPerSecond;
    menuDisplayStyle[0] = tr("after one another");
    menuDisplayStyle[1] = tr("at one go");
    Setup();
}

cSkinDesignerSetup::~cSkinDesignerSetup() {
    config.setupCloseDoReload = true;
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
        Store();
    }
    if (!hadSubMenu && (state == osUnknown || Key == kOk)) {
        if ((Key == kOk && !hadSubMenu)) {
            switch (Key) {
                case kOk: {
                    string itemText = Get(Current())->Text();
                    size_t hit = itemText.find(tr("Skin"));
                    if (hit == 0) {
                        string skin = itemText.substr(strlen(tr("Skin"))+1);
                        state = AddSubMenu(new cSkindesignerSkinSetup(skin, ""));
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
    config.framesPerSecond = framesPerSecond;

    config.InitSetupIterator();
    cSkinSetup *skinSetup = NULL;
    while (skinSetup = config.GetNextSkinSetup()) {
        string skin = skinSetup->GetSkin();
        skinSetup->InitParameterIterator();
        cSkinSetupParameter *param = NULL;
        while (param = skinSetup->GetNextParameter()) {
            cString paramName = cString::sprintf("%s.%s", skin.c_str(), param->name.c_str());
            SetupStore(*paramName, param->value);
            config.UpdateSkinSetupParameter(*paramName, param->value);
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
    SetupStore("FramesPerSecond", framesPerSecond);
}

cOsdItem *cSkinDesignerSetup::InfoItem(const char *label) {
    cOsdItem *item;
    item = new cOsdItem(cString::sprintf("---------------- %s ----------------", tr(label)));
    item->SetSelectable(false);
    return item;
}

void cSkinDesignerSetup::PluginSetup(void) {
    Add(InfoItem(tr("Plugin Setup")));

    Add(new cMenuEditStraItem(tr("Menu Item display method"), &blockFlush, 2, menuDisplayStyle));
    Add(new cMenuEditIntItem(tr("Frames per Second (fading and shifting)"), &framesPerSecond, 10, 100));

    Add(InfoItem(tr("Reruns")));
    Add(new cMenuEditIntItem(tr("Maximum number of reruns to display"), &rerunAmount, 1, 100));
    Add(new cMenuEditIntItem(tr("Minimum timely distance of rerun (in hours)"), &rerunDistance, 0, 1000));
    Add(new cMenuEditIntItem(tr("Limit Channel Numbers"), &rerunMaxChannel, 0, 1000, tr("no limit")));

    Add(InfoItem(tr("Image Loading")));
    Add(new cMenuEditBoolItem(tr("Debug Image Loading"), &debugImageLoading));
    Add(new cMenuEditBoolItem(tr("Limit Channel Logo Cache"), &limitLogoCache));
    Add(new cMenuEditIntItem(tr("Number to cache initially (per size)"), &numLogosPerSizeInitial, 0, 1000));
    Add(new cMenuEditIntItem(tr("Number to cache in maximum"), &numLogosMax, 0, 1000));
}

void cSkinDesignerSetup::SkinSetup(void) {
    Add(InfoItem(tr("Skin Setup")));

    config.InitSkinIterator();
    string skin = "";
    while (config.GetSkin(skin)) {
        cSkinSetup *skinSetup = config.GetSkinSetup(skin);
        if (!skinSetup) {
            Add(new cOsdItem(cString::sprintf("%s %s\t%s", tr("Skin"), skin.c_str(), tr("has no setup"))));
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

    Add(InfoItem(tr("Cache Statistics")));
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

// --- cSkinSetupSubMenu -----------------------------------------------------------

cSkinSetupSubMenu::cSkinSetupSubMenu(string name, string displayText) : cOsdItem(displayText.c_str()) {
    this->name = name;
}

// --- cSkindesignerSkinSetup -----------------------------------------------------------

cSkindesignerSkinSetup::cSkindesignerSkinSetup(string skin, string name)  : 
cOsdMenu(*cString::sprintf("%s: %s \"%s\" %s", trVDR("Setup"), tr("Skin"), skin.c_str(), name.c_str()), 30) {
    SetMenuCategory(mcPluginSetup);
    this->skin = skin;
    this->name = name;
    Set();
}

cSkindesignerSkinSetup::~cSkindesignerSkinSetup() {
}

eOSState cSkindesignerSkinSetup::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
        switch (Key) {
            case kOk: {
                cOsdItem *current = Get(Current());
                cSkinSetupSubMenu *subMenuItem = dynamic_cast<cSkinSetupSubMenu*>(current);
                if (subMenuItem) {
                    state = AddSubMenu(new cSkindesignerSkinSetup(skin, subMenuItem->GetName()));
                    break;
                } else {
                    return osBack;
                }
            } default:
                break;
        }
    }
    return state;
}

void cSkindesignerSkinSetup::Set(void) {
    cSkinSetupMenu *menu = config.GetSkinSetupMenu(skin, name);
    if (!menu) {
        return;
    }
    menu->InitParameterIterator();
    cSkinSetupParameter *param = NULL;
    while (param = menu->GetNextParameter(false)) {
        if (param->type == sptInt) {
            Add(new cMenuEditIntItem(param->displayText.c_str(), &param->value, param->min, param->max));
        } else if (param->type == sptBool) {
            Add(new cMenuEditBoolItem(param->displayText.c_str(), &param->value));
        }
    }

    menu->InitSubmenuIterator();
    cSkinSetupMenu *subMenu = NULL;
    while (subMenu = menu->GetNextSubMenu(false)) {
        Add(new cSkinSetupSubMenu(subMenu->GetName(), subMenu->GetDisplayText()));
    }
}
