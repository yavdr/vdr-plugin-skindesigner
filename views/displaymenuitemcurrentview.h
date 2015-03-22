#ifndef __DISPLAYMENUITEMCURRENTVIEW_H
#define __DISPLAYMENUITEMCURRENTVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayMenuItemCurrentView : public cViewElement {
private:
protected:
    cRect posMenuItem;
    void SetTokensPosMenuItem(void);
    void SetScraperPoster(const cEvent *event, const cRecording *recording=NULL);
public:
    cDisplayMenuItemCurrentView(cTemplateViewElement *tmplCurrent);
    virtual ~cDisplayMenuItemCurrentView();
    void SetPosMenuItem(cRect &pos);
};

class cDisplayMenuItemCurrentMainView: public cDisplayMenuItemCurrentView {
private:
    string number;
    string label;
    string icon;
public:
    cDisplayMenuItemCurrentMainView(cTemplateViewElement *tmplCurrent, string number, string label, string icon);
    virtual ~cDisplayMenuItemCurrentMainView() {};
    void Render(void);
};

class cDisplayMenuItemCurrentSchedulesView: public cDisplayMenuItemCurrentView {
private:
    const cEvent *event;
    const cChannel *channel;
    eTimerMatch timerMatch;
    eMenuCategory cat;
    bool isEpgSearchFav; 
    void ReadSchedules(vector< map<string,string> > *schedulesTokens);
public:
    cDisplayMenuItemCurrentSchedulesView(cTemplateViewElement *tmplCurrent, const cEvent *event, const cChannel *channel, eTimerMatch timerMatch, eMenuCategory cat, bool isEpgSearchFav);
    virtual ~cDisplayMenuItemCurrentSchedulesView() {};
    void Render(void);
};

class cDisplayMenuItemCurrentChannelView: public cDisplayMenuItemCurrentView {
private:
    const cChannel *channel;
    void ReadSchedules(vector< map<string,string> > *schedulesTokens);
public:
    cDisplayMenuItemCurrentChannelView(cTemplateViewElement *tmplCurrent, const cChannel *channel);
    virtual ~cDisplayMenuItemCurrentChannelView() {};
    void Render(void);
};

class cDisplayMenuItemCurrentTimerView: public cDisplayMenuItemCurrentView {
private:
    const cTimer *timer;
public:
    cDisplayMenuItemCurrentTimerView(cTemplateViewElement *tmplCurrent, const cTimer *timer);
    virtual ~cDisplayMenuItemCurrentTimerView() {};
    void Render(void);
};

class cDisplayMenuItemCurrentRecordingView: public cDisplayMenuItemCurrentView {
private:
    const cRecording *recording;
    int level;
    int total;
    int newRecs;
public:
    cDisplayMenuItemCurrentRecordingView(cTemplateViewElement *tmplCurrent, const cRecording *recording, int level, int total, int newRecs);
    virtual ~cDisplayMenuItemCurrentRecordingView() {};
    void Render(void);
};

class cDisplayMenuItemCurrentPluginView: public cDisplayMenuItemCurrentView {
private:
    map<string,vector<map<string,string> > > loopTokens;
public:
    cDisplayMenuItemCurrentPluginView(cTemplateViewElement *tmplCurrent, map <string,string> &plugStringTokens, map <string,int> &plugIntTokens, map<string,vector<map<string,string> > > &pluginLoopTokens);
    virtual ~cDisplayMenuItemCurrentPluginView() {};
    void Render(void);
};

#endif //__DISPLAYMENUITEMCURRENTVIEW_H
