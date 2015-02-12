#ifndef __SKINDESIGNER_H
#define __SKINDESIGNER_H

#include "config.h"
#include "libtemplate/template.h"
#include "libtemplate/xmlparser.h"
#include "displaychannel.h"
#include "displaymenu.h"
#include "displayreplay.h"
#include "displayvolume.h"
#include "displaytracks.h"
#include "displaymessage.h"
#include "displayplugin.h"
#include <vdr/skinlcars.h>

class cSkinDesigner : public cSkin {
private:
    bool init;
    string skin;
    cSkinLCARS *backupSkin;
    bool useBackupSkin;
    cGlobals *globals;
    cTemplate *channelTemplate;
    cTemplate *menuTemplate;
    cTemplate *messageTemplate;
    cTemplate *replayTemplate;
    cTemplate *volumeTemplate;
    cTemplate *audiotracksTemplate;
    map< string, map <int, cTemplate*> > pluginTemplates;
    cSDDisplayMenu *currentMenu;
    void Init(void);
    void ReloadCaches(void);
    void DeleteTemplates(void);
    bool LoadTemplates(void);
    void CacheTemplates(void);
public:
    cSkinDesigner(string skin);
    virtual ~cSkinDesigner(void);
    virtual const char *Description(void);
    virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
    virtual cSkinDisplayMenu *DisplayMenu(void);
    virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
    virtual cSkinDisplayVolume *DisplayVolume(void);
    virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
    virtual cSkinDisplayMessage *DisplayMessage(void);
    virtual cSkinDisplayPlugin *DisplayPlugin(string pluginName, int viewID);
    void ActivateBackupSkin(void) { useBackupSkin = true; };
    void Reload(void);
    void ListAvailableFonts(void);
    bool SetCustomToken(string option);
    void ListCustomTokens(void);
    cSDDisplayMenu *GetDisplayMenu(void) { return currentMenu; };
};

#endif //__SKINDESIGNER_H
