#include <vdr/menu.h>
#include "../config.h"
#include "viewhelpers.h"

cViewHelpers::cViewHelpers(void) {
    devicesInit = false;
    femonReceiver = NULL;
    bitrateVideoLast = bitrateAudioLast = bitrateDolbyLast = 0.0;
}

cViewHelpers::~cViewHelpers() {
    if (devicesInit) {
        delete[] lastSignalStrength;
        delete[] lastSignalQuality;
        delete[] recDevices;
    }
     if (femonReceiver) {
        femonReceiver->Deactivate();
        delete femonReceiver;
    }
}

void cViewHelpers::InitDevices(void) {
    int numDevices = cDevice::NumDevices();
    lastSignalStrength = new int[numDevices];
    lastSignalQuality = new int[numDevices];
    recDevices = new bool[numDevices];
    for (int i=0; i<numDevices; i++) {
        lastSignalStrength[i] = 0;
        lastSignalQuality[i] = 0;
        recDevices[i] = false;
    }
    devicesInit = true;
}

bool cViewHelpers::SetDevices(bool initial, map<string,int> *intTokens, vector<map<string,string> > *devices) {
    int numDevices = cDevice::NumDevices();
    if (!initial) {
        //check if drawing is necessary
        bool changed = false;
        for (int i = 0; i < numDevices; i++) {
            const cDevice *device = cDevice::GetDevice(i);
            if (!device || !device->NumProvidedSystems()) {
                continue;
            }
            if ((device->SignalStrength() != lastSignalStrength[i]) || (device->SignalQuality() != lastSignalQuality[i])) {
                changed = true;
                break;
            }
        }
        if (!changed) {
            return false;
        }
    }
    //check device which currently displays live tv
    int deviceLiveTV = -1;
    cDevice *primaryDevice = cDevice::PrimaryDevice();
    if (primaryDevice) {
        if (!primaryDevice->Replaying() || primaryDevice->Transferring())
            deviceLiveTV = cDevice::ActualDevice()->DeviceNumber();
        else
            deviceLiveTV = primaryDevice->DeviceNumber();
    }

    //check currently recording devices
    for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
        if (!timer->Recording()) {
            continue;
        }
        if (cRecordControl *RecordControl = cRecordControls::GetRecordControl(timer)) {
            const cDevice *recDevice = RecordControl->Device();
            if (recDevice) {
                recDevices[recDevice->DeviceNumber()] = true;
            }
        }
    }
    int actualNumDevices = 0;
    for (int i = 0; i < numDevices; i++) {
        const cDevice *device = cDevice::GetDevice(i);
        if (!device || !device->NumProvidedSystems()) {
            continue;
        }
        
        map< string, string > deviceVals;
        stringstream strNum;
        strNum << actualNumDevices;
        actualNumDevices++;
        deviceVals.insert(pair< string, string >("devices[num]", strNum.str()));
        deviceVals.insert(pair< string, string >("devices[type]", *(device->DeviceType())));
        cCamSlot *camSlot = device->CamSlot();
        int camNumber = -1;
        if (camSlot) {
            camNumber = camSlot->SlotNumber();
            deviceVals.insert(pair< string, string >("devices[hascam]", "1"));
        } else {
            deviceVals.insert(pair< string, string >("devices[hascam]", "0"));
        }
        int signalStrength = device->SignalStrength();
        int signalQuality = device->SignalQuality();
        stringstream strCamNumber;
        strCamNumber << camNumber;
        deviceVals.insert(pair< string, string >("devices[cam]", strCamNumber.str()));
        stringstream strStrength;
        strStrength << signalStrength;
        deviceVals.insert(pair< string, string >("devices[signalstrength]", strStrength.str()));
        stringstream strQuality;
        strQuality << signalQuality;
        deviceVals.insert(pair< string, string >("devices[signalquality]", strQuality.str()));

        deviceVals.insert(pair< string, string >("devices[livetv]", i == deviceLiveTV ? "1" : "0"));
        deviceVals.insert(pair< string, string >("devices[recording]", recDevices[i] ? "1" : "0"));

        const cChannel *channel = device->GetCurrentlyTunedTransponder();
        const cSource *source = (channel) ? Sources.Get(channel->Source()) : NULL;
        if (channel && channel->Number() > 0) {
            stringstream strChanNum;
            strChanNum << channel->Number();
            deviceVals.insert(pair< string, string >("devices[channelnumber]", strChanNum.str()));
            deviceVals.insert(pair< string, string >("devices[channelname]", channel->Name()));
            deviceVals.insert(pair< string, string >("devices[channelid]", *(channel->GetChannelID().ToString())));
            deviceVals.insert(pair< string, string >("devices[istuned]", "1"));
        } else {
            deviceVals.insert(pair< string, string >("devices[channelnumber]", "0"));
            deviceVals.insert(pair< string, string >("devices[channelname]", ""));
            deviceVals.insert(pair< string, string >("devices[channelid]", ""));
            deviceVals.insert(pair< string, string >("devices[istuned]", "0"));
        }
    
        deviceVals.insert(pair< string, string >("devices[source]", source ? source->Description() : ""));

        devices->push_back(deviceVals);

        lastSignalStrength[i] = signalStrength;
        lastSignalQuality[i] = signalQuality;
    }

    intTokens->insert(pair<string, int>("numdevices", actualNumDevices));
    
    return true;
}

void cViewHelpers::InitFemonReceiver(void) {
    const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
    eTrackType track = cDevice::PrimaryDevice()->GetCurrentAudioTrack();
    if (channel) {
        femonReceiver = new cFemonReceiver(channel, 
                                           IS_AUDIO_TRACK(track) ? int(track - ttAudioFirst) : 0, 
                                           IS_DOLBY_TRACK(track) ? int(track - ttDolbyFirst) : 0);
        cDevice::ActualDevice()->AttachReceiver(femonReceiver);
    }
}

void cViewHelpers::ChannelSwitch(const cDevice * device, int channelNumber, bool liveView) {
    if (!femonReceiver)
        return;
    bitrateVideoLast = bitrateAudioLast = bitrateDolbyLast = 0.0;
    eTrackType track = cDevice::PrimaryDevice()->GetCurrentAudioTrack();
    const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());

    if (!liveView || !channelNumber || !channel || channel->Number() != channelNumber)
        return;

    if (femonReceiver) {
        femonReceiver->Deactivate();
        delete femonReceiver;
        femonReceiver = NULL;
    }
    if (channel) {
        femonReceiver = new cFemonReceiver(channel, 
                                           IS_AUDIO_TRACK(track) ? int(track - ttAudioFirst) : 0, 
                                           IS_DOLBY_TRACK(track) ? int(track - ttDolbyFirst) : 0);
        cDevice::ActualDevice()->AttachReceiver(femonReceiver);
    }
}

void cViewHelpers::SetAudioTrack(int Index, const char * const *Tracks) {
    if (!femonReceiver)
        return;
    bitrateVideoLast = bitrateAudioLast = bitrateDolbyLast = 0.0;
    eTrackType track = cDevice::PrimaryDevice()->GetCurrentAudioTrack();
    if (femonReceiver) {
        femonReceiver->Deactivate();
        delete femonReceiver;
        femonReceiver = NULL;
    }
    const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
    if (channel) {
        femonReceiver = new cFemonReceiver(channel, 
                                           IS_AUDIO_TRACK(track) ? int(track - ttAudioFirst) : 0, 
                                           IS_DOLBY_TRACK(track) ? int(track - ttDolbyFirst) : 0);
        cDevice::ActualDevice()->AttachReceiver(femonReceiver);
    }
}

bool cViewHelpers::GetBitrates(double &bitrateVideo, double &bitrateAudio, double &bitrateDolby) {
    bitrateVideo = (int)(femonReceiver->VideoBitrate() / 1024 / 1024 * 100 + 0.5) / 100.0;
    bitrateAudio = (int)(femonReceiver->AudioBitrate() / 1024 * 100 + 0.5) / 100.0;
    bitrateDolby = (int)(femonReceiver->AC3Bitrate() / 1024 * 100 + 0.5) / 100.0;
    if (bitrateVideo != bitrateVideoLast || bitrateAudio != bitrateAudioLast || bitrateDolby != bitrateDolbyLast) {
        bitrateVideoLast = bitrateVideo;
        bitrateAudioLast = bitrateAudio;
        bitrateDolbyLast = bitrateDolby;
        return true;
    }
    return false;
}
