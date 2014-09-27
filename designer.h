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
#include <vdr/skinlcars.h>

class cSkinDesigner : public cSkin {
private:
    cSkinLCARS *backupSkin;
    bool useBackupSkin;
    cRect osdSize;
    std::string osdTheme;
    cGlobals *globals;
    cTemplate *channelTemplate;
    cTemplate *menuTemplate;
    cTemplate *messageTemplate;
    cTemplate *replayTemplate;
    cTemplate *volumeTemplate;
    cTemplate *audiotracksTemplate;
    void DeleteTemplates(void);
    void ReloadCaches(void);
    bool LoadTemplates(void);
    void CacheTemplates(void);
    void SetOSDSize(void);
    bool OsdSizeChanged(void);
    bool ThemeChanged(void);
public:
    cSkinDesigner(void);
    virtual ~cSkinDesigner(void);
    virtual const char *Description(void);
    virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
    virtual cSkinDisplayMenu *DisplayMenu(void);
    virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
    virtual cSkinDisplayVolume *DisplayVolume(void);
    virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
    virtual cSkinDisplayMessage *DisplayMessage(void);
    void Reload(void);
    void ListAvailableFonts(void);
};

#endif //__SKINDESIGNER_H
