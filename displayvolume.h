#ifndef __DISPLAYVOLUME_H
#define __DISPLAYVOLUME_H

#include <vdr/skins.h>
#include "config.h"
#include "libtemplate/template.h"
#include "views/displayvolumeview.h"

class cSDDisplayVolume : public cSkinDisplayVolume {
private:
    cDisplayVolumeView *volumeView;
    bool doOutput;
    bool initial;
public:
    cSDDisplayVolume(cTemplate *volumeTemplate);
    virtual ~cSDDisplayVolume();
    virtual void SetVolume(int Current, int Total, bool Mute);
    virtual void Flush(void);
};

#endif //__DISPLAYVOLUME_H