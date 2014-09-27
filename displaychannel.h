#ifndef __DISPLAYCHANNEL_H
#define __DISPLAYCHANNEL_H

#include <vdr/thread.h>
#include "config.h"
#include "libtemplate/template.h"
#include "views/displaychannelview.h"

class cSDDisplayChannel : public cSkinDisplayChannel {
private:
    cDisplayChannelView *channelView;
    bool doOutput;
    bool initial;
    bool groupSep;
    bool channelChange;
    time_t lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    int lastScreenWidth;
    int currentLast;
    bool showSignal;
    const cEvent *present;
    void SetProgressBar(const cEvent *present);
public:
    cSDDisplayChannel(cTemplate *channelTemplate, bool WithInfo);
    virtual ~cSDDisplayChannel();
    virtual void SetChannel(const cChannel *Channel, int Number);
    virtual void SetEvents(const cEvent *Present, const cEvent *Following);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};
#endif //__DISPLAYCHANNEL_H