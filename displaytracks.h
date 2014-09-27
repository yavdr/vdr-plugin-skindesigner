#ifndef __DISPLAYTRACKS_H
#define __DISPLAYTRACKS_H

#include <vdr/skins.h>
#include <vdr/thread.h>
#include "config.h"
#include "libtemplate/template.h"
#include "views/displayaudiotracksview.h"

class cSDDisplayTracks : public cSkinDisplayTracks {
private:
    cDisplayAudiotracksView *tracksView;
    bool initial;
    int numTracks;
    bool doOutput;
    int currentTrack;
    const char *menuTitle;
public:
    cSDDisplayTracks(cTemplate *audiotracksTemplate, const char *Title, int NumTracks, const char * const *Tracks);
    virtual ~cSDDisplayTracks();
    virtual void SetTrack(int Index, const char * const *Tracks);
    virtual void SetAudioChannel(int AudioChannel);
    virtual void Flush(void);
};


#endif //__DISPLAYTRACKS_H