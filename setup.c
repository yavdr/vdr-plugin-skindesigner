#include "libcore/curlfuncs.h"
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
    InstallSkins();
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
    if (!hadSubMenu && (Key == kOk || Key == kUp || Key == kDown || Key == kLeft || Key == kRight || Key == kRed)) {
        SetHelp(NULL, NULL, NULL, NULL);
        cOsdItem *current = Get(Current());
        cSkinMenuItem *skinMenuItem = dynamic_cast<cSkinMenuItem*>(current);
        if (!skinMenuItem)
            return state;
        eItemType type = skinMenuItem->Type();
        string skinName = skinMenuItem->GetSkinName();
        // KEY OK
        if ((Key == kOk)) {
            if (type == itSkinSetup) {
                state = AddSubMenu(new cSkindesignerSkinSetup(skinName, ""));
            } else if (type == itNoSkinSetup) {
                state = osContinue;
            } else if (type == itSkinRepo) {
                Skins.Message(mtStatus, tr("Downloading Skin Screenshots..."));
                cSkindesignerSkinPreview *prev = new cSkindesignerSkinPreview(skinName);
                Skins.Message(mtStatus, NULL);
                state = AddSubMenu(prev);
            }
        }
        // Menu Moves
        if (Key == kUp || Key == kDown || Key == kLeft || Key == kRight) {
            if (type == itSkinRepo) {
                SetHelp(tr("Install Skin"), NULL, NULL, NULL);
            } else if (type == itSkinSetup || type == itNoSkinSetup) {
                cSkinRepo *repo = config.GetSkinRepo(skinName);
                if (repo && repo->Type() == rtGit) {
                    SetHelp(tr("Update from Git"), NULL, NULL, NULL);
                }
            }
        }
        // KEY RED
        if (Key == kRed) {
            if (type == itSkinRepo) {
                Skins.Message(mtStatus, tr("Installing Skin..."));
                cSkinRepo *skinRepo = config.GetSkinRepo(skinName);
                if (!skinRepo) {
                    return state;
                }
                skinRepo->Install(*config.installerSkinPath, config.vdrThemesPath);
                while (!skinRepo->InstallationFinished()) {
                    cCondWait::SleepMs(50);
                }
                bool ok = skinRepo->SuccessfullyInstalled();
                if (ok) {
                    config.AddNewSkinRef(skinName);
                    Skins.Message(mtStatus, tr("Skin successfully installed"));
                    cCondWait::SleepMs(1000);
                    state = osEnd;
                } else {
                    Skins.Message(mtStatus, tr("Skin NOT successfully installed"));
                    state = osContinue;
                 }
            } else if (type == itSkinSetup || type == itNoSkinSetup) {
                cSkinRepo *skinRepo = config.GetSkinRepo(skinName);
                if (!skinRepo || skinRepo->Type() != rtGit) {
                    Skins.Message(mtStatus, tr("No Git Repsoitory available"));
                    return state;
                }
                Skins.Message(mtStatus, tr("Updating Skin from Git..."));
                skinRepo->Update(*config.installerSkinPath);
                while (!skinRepo->InstallationFinished()) {
                    cCondWait::SleepMs(50);
                }
                bool ok = skinRepo->SuccessfullyUpdated();
                if (ok) {
                    Skins.Message(mtStatus, tr("Skin successfully updated"));
                    cCondWait::SleepMs(1000);
                    state = osEnd;
                } else {
                    Skins.Message(mtStatus, tr("Skin already up to date"));
                    state = osContinue;
                }
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

void cSkinDesignerSetup::SkinSetup(void) {
    Add(InfoItem(tr("Skin Setup")));

    config.InitSkinIterator();
    string skin = "";
    while (config.GetSkin(skin)) {
        cSkinSetup *skinSetup = config.GetSkinSetup(skin);
        if (!skinSetup) {
            Add(new cSkinMenuItem(skin.c_str(), *cString::sprintf("%s %s\t(%s)", tr("Skin"), skin.c_str(), tr("has no setup")), itNoSkinSetup));
        } else {
            Add(new cSkinMenuItem(skin.c_str(), *cString::sprintf("%s %s", tr("Skin"), skin.c_str()), itSkinSetup));
        }
    }
}

void cSkinDesignerSetup::InstallSkins(void) {
    Add(InfoItem(tr("Install new skins")));

    config.InitSkinRepoIterator();
    cSkinRepo *repo = NULL;
    while (repo = config.GetNextSkinRepo()) {
        if (config.SkinInstalled(repo->Name()))
            continue;
        Add(new cSkinMenuItem(repo->Name(), *cString::sprintf("%s %s", tr("Preview Skin"), repo->Name().c_str()), itSkinRepo));
    }
}

// --- cSkinMenuItem -----------------------------------------------------------
cSkinMenuItem::cSkinMenuItem(string skinName, string displayText, eItemType type) : cOsdItem(displayText.c_str()) {
    this->skinName = skinName;
    this->type = type;
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
            }
            case kRed: {
                cSkinRepo *skinRepo = config.GetSkinRepo(skin);
                if (!skinRepo) {
                    Skins.Message(mtStatus, tr("No Git Repsoitory available"));
                    return state;
                }
                Skins.Message(mtStatus, tr("Updating Skin from Git..."));
                skinRepo->Update(*config.installerSkinPath);
                while (!skinRepo->InstallationFinished()) {
                    cCondWait::SleepMs(50);
                }
                bool ok = skinRepo->SuccessfullyUpdated();
                if (ok) {
                    Skins.Message(mtStatus, tr("Skin successfully updated"));
                    cCondWait::SleepMs(1000);
                    state = osEnd;
                } else {
                    Skins.Message(mtStatus, tr("Skin already up to date"));
                    state = osContinue;
                }
            }
            default:
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
    
    cSkinRepo *repo = config.GetSkinRepo(skin);
    if (repo && repo->Type() == rtGit) {
        SetHelp(tr("Update from Git"), NULL, NULL, NULL);
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

// --- cSkindesignerSkinPreview -----------------------------------------------------------

cSkindesignerSkinPreview::cSkindesignerSkinPreview(string skinName)  : 
cSkindesignerOsdMenu(*cString::sprintf("%s: %s \"%s\"", trVDR("Preview"), tr("Skin"), skinName.c_str())) {
    this->skinName = skinName;
    SetPluginName("setup");
    FirstCallCleared();
    Set();
}

cSkindesignerSkinPreview::~cSkindesignerSkinPreview() {
}

eOSState cSkindesignerSkinPreview::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    switch (Key) {
        case kOk:
        case kBack:
            state = osBack;
            break;
        case kLeft: {
            TextKeyLeft();
            state = osContinue;
            break;
        } case kRight: {
            TextKeyRight();
            state = osContinue;            
            break;
        } case kUp: {
            TextKeyUp();
            state = osContinue;            
            break;
        } case kDown: {
            TextKeyDown();
            state = osContinue;
            break;
        } case kRed: {
            Skins.Message(mtStatus, tr("Installing Skin..."));
            bool ok = InstallSkin();
            if (ok)
                Skins.Message(mtStatus, tr("Skin successfully installed"));
            else
                Skins.Message(mtStatus, tr("Skin NOT successfully installed"));
            cCondWait::SleepMs(1000);
            state = osEnd;
            break;
        } default:
            break;
    }
    return state;
}

void cSkindesignerSkinPreview::Display(void) {
    SetHelp(tr("Install Skin"), NULL, NULL, NULL);
    skindesignerapi::cSkindesignerOsdMenu::Display();    
}

void cSkindesignerSkinPreview::Set(void) {
    SetPluginMenu(0, skindesignerapi::mtText);
    ClearTokens();
    Clear();

    cSkinRepo *skinRepo = config.GetSkinRepo(skinName);
    if (!skinRepo) {
        esyslog("skindesigner: no valid skin repository found for skin %s", skinName.c_str());
        return;
    }

    AddStringToken("menuheader", *cString::sprintf("%s: %s \"%s\"", trVDR("Preview"), tr("Skin"), skinName.c_str()));
    AddStringToken("skinname", skinName);
    AddStringToken("author", skinRepo->Author());
    
    stringstream plainText;
    plainText << *cString::sprintf("%s: %s \"%s\"", trVDR("Preview"), tr("Skin"), skinName.c_str()) << "\n\n";
    plainText << tr("Author") << ": " << skinRepo->Author() << "\n";

    plainText << tr("Used Fonts") << ": \n";
    vector<string> specialFonts = skinRepo->SpecialFonts();
    for (vector<string>::iterator it = specialFonts.begin(); it != specialFonts.end(); it++) {
        map<string,string> usedFonts;
        usedFonts.insert(pair<string,string>("fonts[name]", *it));
        usedFonts.insert(pair<string,string>("fonts[installed]", CheckFontInstalled(*it)));
        AddLoopToken("fonts", usedFonts);
        plainText << *it << "\n";
    }
    
    plainText << tr("Supported Plugins") << ": \n";
    vector<string> supportedPlugins = skinRepo->SupportedPlugins();
    for (vector<string>::iterator it = supportedPlugins.begin(); it != supportedPlugins.end(); it++) {
        map<string,string> plugins;
        plugins.insert(pair<string,string>("plugins[name]", *it));
        AddLoopToken("plugins", plugins);
        plainText << *it << "\n";
    }

    SetText(plainText.str().c_str());

    vector< pair < string, string > > screenshots = skinRepo->Screenshots();
    int i = 0;
    for (vector< pair < string, string > >::iterator it = screenshots.begin(); it != screenshots.end(); it++) {
        string url = it->second;
        string imgType = ".jpg";
        if (url.find(".png") != string::npos)
            imgType = ".png";
        stringstream tempName;
        tempName << "/tmp/screenshot_" << skinName << "_" << i++ << imgType;
        dsyslog("skindesigner: download screenshot name %s url %s", tempName.str().c_str(), url.c_str());
        CurlGetUrlFile(url.c_str(), tempName.str().c_str());
        map<string,string> img;
        img.insert(pair<string,string>("screenshots[desc]", it->first));
        img.insert(pair<string,string>("screenshots[path]", tempName.str()));
        AddLoopToken("screenshots", img);
    }
}

bool cSkindesignerSkinPreview::InstallSkin(void) {
    cSkinRepo *skinRepo = config.GetSkinRepo(skinName);
    if (!skinRepo) {
        esyslog("skindesigner: no valid skin repository found for skin %s", skinName.c_str());
        return false;
    }
    skinRepo->Install(*config.installerSkinPath, config.vdrThemesPath);
    while (!skinRepo->InstallationFinished()) {
        cCondWait::SleepMs(50);
    }
    bool ok = skinRepo->SuccessfullyInstalled();
    if (ok) {
        config.AddNewSkinRef(skinName);
    }
    return ok;
}

string cSkindesignerSkinPreview::CheckFontInstalled(string fontName) {
    if (fontManager->FontInstalled(fontName))
        return "1";
    return "0";
}
