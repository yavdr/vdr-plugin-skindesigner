#ifndef __VIEWHELPERS_H
#define __VIEWHELPERS_H

class cViewHelpers {
private:
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
protected:
    void InitDevices(void);
    bool SetDevices(bool initial, map<string,int> *intTokens, vector<map<string,string> > *devices);
    bool SetSignal(map < string, int > &intTokens);
    bool CheckNewMails(void);
    void SetScraperTokens(const cEvent *event, const cRecording *recording, map < string, string > &stringTokens, map < string, int > &intTokens, map < string, vector< map< string, string > > > &loopTokens);
    void SetPosterBanner(const cEvent *event, map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetTime(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetDate(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetCurrentWeatherTokens(map < string, string > &stringTokens, map < string, int > &intTokens);
    void SetTimers(map<string,int> *intTokens, map<string,string> *stringTokens, vector<map<string,string> > *timers);
    void SetMenuHeader(eMenuCategory cat, string menuTitle, map < string, string > &stringTokens, map < string, int > &intTokens);
    void SetDiscUsage(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetSystemLoad(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetSystemMemory(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetSystemTemperatures(map < string, string > &stringTokens, map < string, int > &intTokens);
    void SetCurrentSchedule(map < string, string > &stringTokens, map < string, int > &intTokens);
public:
    cViewHelpers(void);
    virtual ~cViewHelpers(void);
};

#endif //__VIEWHELPERS_H