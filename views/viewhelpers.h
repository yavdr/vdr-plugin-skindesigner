#ifndef __VIEWHELPERS_H
#define __VIEWHELPERS_H

class cViewHelpers {
private:
    int numDevices;
    bool devicesInit;
    int* lastSignalStrength;
    int* lastSignalQuality;
    bool* recDevices;
    int lastSecond;
    int lastMinute;
protected:
    void InitDevices(void);
    bool SetDevices(bool initial, map<string,int> *intTokens, vector<map<string,string> > *devices);
    bool CheckNewMails(void);
    void SetScraperTokens(const cEvent *event, const cRecording *recording, map < string, string > &stringTokens, map < string, int > &intTokens, map < string, vector< map< string, string > > > &loopTokens);
    bool SetTime(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetDate(map < string, string > &stringTokens, map < string, int > &intTokens);
    bool SetCurrentWeatherTokens(map < string, string > &stringTokens, map < string, int > &intTokens);
public:
    cViewHelpers(void);
    virtual ~cViewHelpers(void);
};

#endif //__VIEWHELPERS_H