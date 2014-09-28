#include "designer.h"
#include "libcore/helpers.h"

cSkinDesigner::cSkinDesigner(void) : cSkin("skindesigner", &::Theme) {
    backupSkin = NULL;
    useBackupSkin = false;

    SetOSDSize();
    osdSkin =  Setup.OSDSkin;
    osdTheme = Setup.OSDTheme;

    config.SetPathes();
    config.SetChannelLogoSize();
    config.CheckDecimalPoint();
    fontManager = new cFontManager();
    imgCache = new cImageCache();

    globals = NULL;
    channelTemplate = NULL;
    menuTemplate = NULL;
    messageTemplate = NULL;
    replayTemplate = NULL;
    volumeTemplate = NULL;
    audiotracksTemplate = NULL;

    cStopWatch watch;
    bool ok = LoadTemplates();
    if (!ok) {
        esyslog("skindesigner: error during loading of templates - using LCARS as backup");
        backupSkin = new cSkinLCARS();
        useBackupSkin = true;
    } else {
        CacheTemplates();
        watch.Stop("templates loaded and cache created");
    }
}

cSkinDesigner::~cSkinDesigner(void) {
    if (globals)
        delete globals;
    DeleteTemplates();
    if (backupSkin)
        delete backupSkin;
}

const char *cSkinDesigner::Description(void) {
  return "SkinDesigner";
}

cSkinDisplayChannel *cSkinDesigner::DisplayChannel(bool WithInfo) {
    cSkinDisplayChannel *displayChannel = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        displayChannel = new cSDDisplayChannel(channelTemplate, WithInfo);
    } else {
        displayChannel = backupSkin->DisplayChannel(WithInfo);
    }
    return displayChannel;
}

cSkinDisplayMenu *cSkinDesigner::DisplayMenu(void) {
    cSkinDisplayMenu *displayMenu = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        firstDisplay = false;
        displayMenu = new cSDDisplayMenu(menuTemplate);
    } else {
        displayMenu = backupSkin->DisplayMenu();
    }
    return displayMenu;
}

cSkinDisplayReplay *cSkinDesigner::DisplayReplay(bool ModeOnly) {
    cSkinDisplayReplay *displayReplay = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        displayReplay = new cSDDisplayReplay(replayTemplate, ModeOnly);
    } else {
        displayReplay = backupSkin->DisplayReplay(ModeOnly);
    }
    return displayReplay;
}

cSkinDisplayVolume *cSkinDesigner::DisplayVolume(void) {
    cSkinDisplayVolume *displayVolume = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        displayVolume = new cSDDisplayVolume(volumeTemplate);
    } else {
        displayVolume = backupSkin->DisplayVolume();
    }
    return displayVolume;
}

cSkinDisplayTracks *cSkinDesigner::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks) {
    cSkinDisplayTracks *displayTracks = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        displayTracks = new cSDDisplayTracks(audiotracksTemplate, Title, NumTracks, Tracks);
    } else {
        displayTracks = backupSkin->DisplayTracks(Title, NumTracks, Tracks);
    }
    return displayTracks;
}

cSkinDisplayMessage *cSkinDesigner::DisplayMessage(void) {
    cSkinDisplayMessage *displayMessage = NULL;
    if (!useBackupSkin) {
        ReloadCaches();
        displayMessage = new cSDDisplayMessage(messageTemplate);
    } else {
        displayMessage = backupSkin->DisplayMessage();
    }
    return displayMessage;
}

void cSkinDesigner::Reload(void) {
    dsyslog("skindesigner: forcing full reload of templates");
    if (cOsd::IsOpen()) {
        esyslog("skindesigner: OSD is open, close first!");
        return;
    }
    DeleteTemplates();
    cStopWatch watch;
    bool ok = LoadTemplates();
    if (!ok) {
        esyslog("skindesigner: error during loading of templates - using LCARS as backup");
        if (!backupSkin)
            backupSkin = new cSkinLCARS();
        useBackupSkin = true;
    } else {
        CacheTemplates();
        useBackupSkin = false;        
        watch.Stop("templates reloaded and cache created");
    }
}

void cSkinDesigner::ListAvailableFonts(void) {
    fontManager->ListAvailableFonts();
}

/*********************************************************************************
* PRIVATE FUNCTIONS
*********************************************************************************/    
void cSkinDesigner::DeleteTemplates(void) {
    if (channelTemplate) {
        delete channelTemplate;
        channelTemplate = NULL;
    }
    if (menuTemplate) {
        delete menuTemplate;
        menuTemplate = NULL;
    }
    if (messageTemplate) {
        delete messageTemplate;
        messageTemplate = NULL;
    }
    if (replayTemplate) {
        delete replayTemplate;
        replayTemplate = NULL;
    }
    if (volumeTemplate) {
        delete volumeTemplate;
        volumeTemplate = NULL;
    }
    if (audiotracksTemplate) {
        delete audiotracksTemplate;
        audiotracksTemplate = NULL;
    }
}

bool cSkinDesigner::LoadTemplates(void) {
    
    globals = new cGlobals();
    bool ok = globals->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error parsing globals, aborting");
        return false;
    }

    DeleteTemplates();

    channelTemplate = new cTemplate(vtDisplayChannel);
    channelTemplate->SetGlobals(globals);
    ok = channelTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaychannel template, aborting");
        DeleteTemplates();
        return false;
    }
    channelTemplate->Translate();

    menuTemplate = new cTemplate(vtDisplayMenu);
    menuTemplate->SetGlobals(globals);
    ok = menuTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaymenu template, aborting");
        DeleteTemplates();
        return false;
    }
    menuTemplate->Translate();

    messageTemplate = new cTemplate(vtDisplayMessage);
    messageTemplate->SetGlobals(globals);
    ok = messageTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displaymessage template, aborting");
        DeleteTemplates();
        return false;
    }
    messageTemplate->Translate();

    replayTemplate = new cTemplate(vtDisplayReplay);
    replayTemplate->SetGlobals(globals);
    ok = replayTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayreplay template, aborting");
        DeleteTemplates();
        return false;
    }
    replayTemplate->Translate();

    volumeTemplate = new cTemplate(vtDisplayVolume);
    volumeTemplate->SetGlobals(globals);
    ok = volumeTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayvolume template, aborting");
        DeleteTemplates();
        return false;
    }
    volumeTemplate->Translate();

    audiotracksTemplate = new cTemplate(vtDisplayAudioTracks);
    audiotracksTemplate->SetGlobals(globals);
    ok = audiotracksTemplate->ReadFromXML();
    if (!ok) {
        esyslog("skindesigner: error reading displayaudiotracks template, aborting");
        DeleteTemplates();
        return false;
    }
    audiotracksTemplate->Translate();

    dsyslog("skindesigner: templates successfully validated and parsed");        
    return true;
}

void cSkinDesigner::CacheTemplates(void) {
    channelTemplate->PreCache();
    menuTemplate->PreCache();
    messageTemplate->PreCache();
    replayTemplate->PreCache();
    volumeTemplate->PreCache();
    audiotracksTemplate->PreCache();
    dsyslog("skindesigner: templates cached");
    fontManager->CacheFonts(channelTemplate);
    fontManager->CacheFonts(menuTemplate);
    fontManager->CacheFonts(messageTemplate);
    fontManager->CacheFonts(replayTemplate);
    fontManager->CacheFonts(volumeTemplate);
    fontManager->CacheFonts(audiotracksTemplate);
    dsyslog("skindesigner: fonts cached");
    dsyslog("skindesigner: caching images...");
    imgCache->Clear();
    channelTemplate->CacheImages();
    menuTemplate->CacheImages();
    messageTemplate->CacheImages();
    replayTemplate->CacheImages();
    volumeTemplate->CacheImages();
    audiotracksTemplate->CacheImages();
    imgCache->Debug(false);
}

void cSkinDesigner::ReloadCaches(void) {
    if (OsdSizeChanged() || ThemeChanged()) {
        cStopWatch watch;
        bool ok = LoadTemplates();
        if (ok) {
            CacheTemplates();
        }
        watch.Stop("templates reloaded and cache recreated");
    }
}

void cSkinDesigner::SetOSDSize(void) {
    osdSize.SetWidth(cOsd::OsdWidth());
    osdSize.SetHeight(cOsd::OsdHeight());
    osdSize.SetX(cOsd::OsdLeft());
    osdSize.SetY(cOsd::OsdTop());
}

bool cSkinDesigner::OsdSizeChanged(void) {
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

bool cSkinDesigner::ThemeChanged(void) {
    bool changed = false;
    if (osdSkin.compare(Setup.OSDSkin) != 0) {
        osdSkin = Setup.OSDSkin;
        changed = true;
    }
    if (osdTheme.compare(Setup.OSDTheme) != 0) {
        osdTheme = Setup.OSDTheme;
        changed = true;
    }
    return changed;
}
