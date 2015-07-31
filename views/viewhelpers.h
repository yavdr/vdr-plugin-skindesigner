#ifndef __VIEWHELPERS_H
#define __VIEWHELPERS_H

#include "../services/dvbapi.h"

typedef map<string,string> stringmap;
typedef map<string,int> intmap;
typedef pair<string,string> stringpair;

class cViewHelpers {
private:
    cMutex mutexDevices;
    int numDevices;
    bool devicesInit;
    int* lastSignalStrength;
    int* lastSignalQuality;
    bool* recDevices;
    int lSignalDisplay;
    int lSignalStrength;
    int lSignalQuality;
    int lastSecond;
    int lastMinute;
    double lastSystemLoad;
    int lastMemUsage;
    string lastVdrCPU;
    string lastVdrMEM;
    sDVBAPIEcmInfo lastEcmInfo;
    void RecName(string &path, string &name, string &folder);
    void RecPoster(const cRecording *rec, int &posterWidth, int &posterHeight, string &path, bool &hasPoster);
    void SetCurrentScheduleFromChannel(const cChannel *channel, stringmap &stringTokens, intmap &intTokens);
    void SetCurrentScheduleFromRecording(const cRecording *recording, stringmap &stringTokens, intmap &intTokens);
    bool CompareECMInfos(sDVBAPIEcmInfo *ecmInfo);
protected:
    void InitDevices(void);
    bool SetDevices(bool initial, bool light, intmap *intTokens, vector<stringmap> *devices);
    bool CheckNewMails(void);
    int GetLiveBuffer(void);
    void SetScraperTokens(const cEvent *event, const cRecording *recording, stringmap &stringTokens, intmap &intTokens, map < string, vector<stringmap> > &loopTokens);
    void SetPosterBanner(const cEvent *event, stringmap &stringTokens, intmap &intTokens);
    void SetPosterBannerV2(const cRecording *recording, stringmap &stringTokens, intmap &intTokens);
    void SetTimers(map<string,int> *intTokens, map<string,string> *stringTokens, vector<stringmap> *timers);
    void SetLastRecordings(map<string,int> *intTokens, map<string,string> *stringTokens, vector<stringmap> *lastRecordings);
    void SetMenuHeader(eMenuCategory cat, string menuTitle, stringmap &stringTokens, intmap &intTokens);
    void SetCurrentSchedule(string recName, stringmap &stringTokens, intmap &intTokens);
    bool SetEcmInfos(int channelSid, stringmap &stringTokens, intmap &intTokens);
    void SetMenuMessage(eMessageType type, string text, stringmap &stringTokens, intmap &intTokens);
public:
    cViewHelpers(void);
    virtual ~cViewHelpers(void);
    bool SetDate                (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetTime                (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetSignal              (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetCurrentWeatherTokens(bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetDiscUsage           (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetSystemLoad          (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetSystemMemory        (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetSystemTemperatures  (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetVDRStats            (bool forced, stringmap &stringTokens, intmap &intTokens);
    bool SetDummy               (bool forced, stringmap &stringTokens, intmap &intTokens);
};

#endif //__VIEWHELPERS_H