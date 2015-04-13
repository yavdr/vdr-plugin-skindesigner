/*
 * skindesigner.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <getopt.h>
#include <vdr/plugin.h>
#include "libskindesignerapi/skindesignerapi.h"

#define DEFINE_CONFIG 1
#include "config.h"
#include "designer.h"
#include "setup.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20000 
#error "VDR-2.0.0 API version or greater is required!"
#endif


static const char *VERSION        = "0.4.3";
static const char *DESCRIPTION    = trNOOP("Skin Designer");

class cPluginSkinDesigner : public cPlugin, public skindesignerapi::SkindesignerAPI {
private:
    vector<cSkinDesigner*> skins;
    string libskindesignerApiVersion;
protected:
    bool ServiceRegisterPlugin(skindesignerapi::cPluginStructure *plugStructure);
    skindesignerapi::ISDDisplayMenu *ServiceGetDisplayMenu(void);
    skindesignerapi::ISkinDisplayPlugin *ServiceGetDisplayPlugin(string pluginName, int viewID, int subViewID);
public:
    cPluginSkinDesigner(void);
    virtual ~cPluginSkinDesigner();
    virtual const char *Version(void) { return VERSION; }
    virtual const char *Description(void) { return tr(DESCRIPTION); }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual time_t WakeupTime(void);
    virtual const char *MainMenuEntry(void) {return NULL;}
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginSkinDesigner::cPluginSkinDesigner(void) {
    libskindesignerApiVersion = "undefined";
}

cPluginSkinDesigner::~cPluginSkinDesigner() {
}

const char *cPluginSkinDesigner::CommandLineHelp(void) {
  return
         "  -s <SKINPATH>, --skinpath=<SKINPATH> Set directory where xml skins are stored\n"
         "  -l <LOGOPATH>, --logopath=<LOGOPATH> Set directory where a common logo set for all skins is stored\n"
         "  -e <EPGIMAGESPATH>, --epgimages=<IMAGESPATH> Set directory where epgimages are stored\n";
}

bool cPluginSkinDesigner::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
    static const struct option long_options[] = {
        { "epgimages", required_argument, NULL, 'e' },
        { "logopath", required_argument, NULL, 'l' },
        { "skinpath", required_argument, NULL, 's' },
        { 0, 0, 0, 0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "e:s:l:", long_options, NULL)) != -1) {
        switch (c) {
            case 'e':
                config.SetEpgImagePath(cString(optarg));
                break;
            case 'l':
                config.SetLogoPath(cString(optarg));
                break;
            case 's':
                config.SetSkinPath(cString(optarg));
                break;
            default:
                return false;
        }
    }
    return true;
}

bool cPluginSkinDesigner::Initialize(void) {
    return true;
}

bool cPluginSkinDesigner::Start(void) {
    cXmlParser::InitLibXML();
    cImageImporterSVG::InitLibRSVG();
    bool trueColorAvailable = true;
    
    if (!cOsdProvider::SupportsTrueColor()) {
        esyslog("skindesigner: No TrueColor OSD found! Using default Skin LCARS!");
        trueColorAvailable = false;
    } else
        dsyslog("skindesigner: TrueColor OSD found");

    libskindesignerApiVersion = LIBSKINDESIGNERAPIVERSION;
    dsyslog("skindesigner: using libskindesigner API Version %s", libskindesignerApiVersion.c_str());

    config.SetOsdLanguage();
    config.SetPathes();
    config.ReadSkins();
    config.InitSkinIterator();
    string skin = "";
    while (config.GetSkin(skin)) {
        config.ReadSkinSetup(skin);
        cSkinDesigner *newSkin = new cSkinDesigner(skin);
        skins.push_back(newSkin);
        if (!trueColorAvailable) {
            newSkin->ActivateBackupSkin();
        }
    }
    config.TranslateSetup();
    config.SetSkinSetupParameters();

    if (skins.size() == 0) {
        esyslog("skindesigner: no skins found! Using default Skin LCARS!");
    }
    return true;
}

void cPluginSkinDesigner::Stop(void) {
    delete imgCache;
    delete fontManager;
    cXmlParser::CleanupLibXML();
}

void cPluginSkinDesigner::Housekeeping(void) {
}

void cPluginSkinDesigner::MainThreadHook(void) {
}

cString cPluginSkinDesigner::Active(void) {
    return NULL;
}

time_t cPluginSkinDesigner::WakeupTime(void) {
    return 0;
}

cOsdObject *cPluginSkinDesigner::MainMenuAction(void) {
    return NULL;
}

cMenuSetupPage *cPluginSkinDesigner::SetupMenu(void) {
    return new cSkinDesignerSetup();
}

bool cPluginSkinDesigner::SetupParse(const char *Name, const char *Value) {
    return config.SetupParse(Name, Value);
}

bool cPluginSkinDesigner::Service(const char *Id, void *Data) {
    return false;
}

const char **cPluginSkinDesigner::SVDRPHelpPages(void) {
    static const char *HelpPages[] = {
        "RELD\n"
        "    force reload of templates and caches",
        "SCTK\n"
        "    Set custom Token name = value",
        "LCTK\n"
        "    List custom Tokens",
        "LSTF\n"
        "    List available Fonts",
        0
    };
    return HelpPages;
}

cString cPluginSkinDesigner::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    
    cSkinDesigner *activeSkin = NULL;
    for (vector<cSkinDesigner*>::iterator skin = skins.begin(); skin != skins.end(); skin++) {
        string activeSkinName = Setup.OSDSkin;
        string currentSkinName = (*skin)->Description();
        if (!currentSkinName.compare(activeSkinName)) {
            activeSkin = *skin;
            break;
        }
    }

    if (!activeSkin) {
        ReplyCode = 550;
        return "";
    }

    if (strcasecmp(Command, "RELD") == 0) {
        config.ClearSkinSetups();
        config.InitSkinIterator();
        string skin = "";
        while (config.GetSkin(skin)) {
            config.ReadSkinSetup(skin);
        }
        config.TranslateSetup();
        config.SetSkinSetupParameters();
        activeSkin->Reload();
        ReplyCode = 250;
        return "SKINDESIGNER reload of templates and caches forced.";
    } else if (strcasecmp(Command, "LSTF") == 0) {
        activeSkin->ListAvailableFonts();
        ReplyCode = 250;
        return "SKINDESIGNER available fonts listed in syslog.";
    } else if (strcasecmp(Command, "SCTK") == 0) {
        if (!Option) {
            ReplyCode = 501;
            return "SKINDESIGNER SCTK Error: no Token name = value set";
        }
        bool optionOk = activeSkin->SetCustomToken(Option);
        if (optionOk) {
            ReplyCode = 250;
            return cString::sprintf("SKINDESIGNER Set custom Token %s", Option);
        } else {
            ReplyCode = 501;
            return cString::sprintf("SKINDESIGNER Invalid custom Token %s", Option);
        }
    } else if (strcasecmp(Command, "LCTK") == 0) {
        activeSkin->ListCustomTokens();
        ReplyCode = 250;
        return "SKINDESIGNER Custom Tokens listed in Log";
    }
    ReplyCode = 502;
    return "";
}

bool cPluginSkinDesigner::ServiceRegisterPlugin(skindesignerapi::cPluginStructure *plugStructure) {
    if (plugStructure->menus.size() < 1 && plugStructure->views.size() < 1) {
        esyslog("skindesigner: error - plugin without menus or views registered");
        return false;
    }
    dsyslog("skindesigner: plugin %s uses libskindesigner API Version %s", plugStructure->name.c_str(), plugStructure->libskindesignerAPIVersion.c_str());
    config.AddPluginMenus(plugStructure->name, plugStructure->menus);
    config.AddPluginViews(plugStructure->name, plugStructure->views, plugStructure->subViews, plugStructure->viewElements, plugStructure->viewGrids);
    if (plugStructure->menus.size() > 0)
        dsyslog("skindesigner: plugin %s has registered %ld menus", plugStructure->name.c_str(), plugStructure->menus.size());
    if (plugStructure->views.size() > 0)
        dsyslog("skindesigner: plugin %s has registered %ld views", plugStructure->name.c_str(), plugStructure->views.size());
    return true;
}

skindesignerapi::ISDDisplayMenu *cPluginSkinDesigner::ServiceGetDisplayMenu(void) {
    cSkin *current = Skins.Current();
    for (vector<cSkinDesigner*>::iterator skin = skins.begin(); skin != skins.end(); skin++) {
        if (*skin == current) {
            cSDDisplayMenu *displayMenu = (*skin)->GetDisplayMenu();
            if (displayMenu) {
                return displayMenu;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

skindesignerapi::ISkinDisplayPlugin *cPluginSkinDesigner::ServiceGetDisplayPlugin(string pluginName, int viewID, int subViewID) {
    if (pluginName.size() == 0 || viewID < 0)
        return NULL;
    cSkin *current = Skins.Current();
    for (vector<cSkinDesigner*>::iterator skin = skins.begin(); skin != skins.end(); skin++) {
        if (*skin == current) {
            cSkinDisplayPlugin *displayPlugin = (*skin)->DisplayPlugin(pluginName, viewID, subViewID);
            if (displayPlugin) {
                return displayPlugin;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

VDRPLUGINCREATOR(cPluginSkinDesigner); // Don't touch this!
