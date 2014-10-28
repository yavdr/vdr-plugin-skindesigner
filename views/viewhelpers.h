#ifndef __VIEWHELPERS_H
#define __VIEWHELPERS_H

#include <vdr/status.h>
#include "../libcore/femonreceiver.h"

class cViewHelpers : public cStatus {
private:
    bool devicesInit;
    int* lastSignalStrength;
    int* lastSignalQuality;
    bool* recDevices;
    cFemonReceiver *femonReceiver;
    double bitrateVideoLast;
    double bitrateAudioLast;
    double bitrateDolbyLast;
protected:
    virtual void ChannelSwitch(const cDevice *device, int channelNumber, bool liveView);
    virtual void SetAudioTrack(int Index, const char * const *Tracks);
    void InitDevices(void);
    bool SetDevices(bool initial, map<string,int> *intTokens, vector<map<string,string> > *devices);
    void InitFemonReceiver(void);
    bool GetBitrates(double &bitrateVideo, double &bitrateAudio, double &bitrateDolby);
public:
    cViewHelpers(void);
    virtual ~cViewHelpers(void);
};

#endif //__VIEWHELPERS_H