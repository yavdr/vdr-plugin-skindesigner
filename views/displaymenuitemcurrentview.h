#ifndef __DISPLAYMENUITEMCURRENTVIEW_H
#define __DISPLAYMENUITEMCURRENTVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayMenuItemCurrentView : public cView {
private:
protected:
    int delay;
    map < string, string > stringTokens;
    map < string, int > intTokens;
    cRect posMenuItem;
    void SetTokensPosMenuItem(void);
    void SetScraperPoster(const cEvent *event, const cRecording *recording=NULL);
public:
    cDisplayMenuItemCurrentView(cTemplateViewElement *tmplCurrent);
    virtual ~cDisplayMenuItemCurrentView();
    void SetPosMenuItem(cRect &pos);
    virtual void Prepare(void) {};
    virtual void Render(void) {};
    virtual void Clear(void) {};
};

class cDisplayMenuItemCurrentMainView: public cDisplayMenuItemCurrentView {
private:
    string number;
    string label;
    string icon;
    void Action(void);
public:
    cDisplayMenuItemCurrentMainView(cTemplateViewElement *tmplCurrent, string number, string label, string icon);
    virtual ~cDisplayMenuItemCurrentMainView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

class cDisplayMenuItemCurrentSchedulesView: public cDisplayMenuItemCurrentView {
private:
    const cEvent *event;
    const cChannel *channel;
    eTimerMatch timerMatch;
    eMenuCategory cat;
    void Action(void);
    void ReadSchedules(vector< map<string,string> > *schedulesTokens);
public:
    cDisplayMenuItemCurrentSchedulesView(cTemplateViewElement *tmplCurrent, const cEvent *event, const cChannel *channel, eTimerMatch timerMatch, eMenuCategory cat);
    virtual ~cDisplayMenuItemCurrentSchedulesView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

class cDisplayMenuItemCurrentChannelView: public cDisplayMenuItemCurrentView {
private:
    const cChannel *channel;
    void Action(void);
    void ReadSchedules(vector< map<string,string> > *schedulesTokens);
public:
    cDisplayMenuItemCurrentChannelView(cTemplateViewElement *tmplCurrent, const cChannel *channel);
    virtual ~cDisplayMenuItemCurrentChannelView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

class cDisplayMenuItemCurrentTimerView: public cDisplayMenuItemCurrentView {
private:
    const cTimer *timer;
    void Action(void);
public:
    cDisplayMenuItemCurrentTimerView(cTemplateViewElement *tmplCurrent, const cTimer *timer);
    virtual ~cDisplayMenuItemCurrentTimerView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

class cDisplayMenuItemCurrentRecordingView: public cDisplayMenuItemCurrentView {
private:
    const cRecording *recording;
    int level;
    int total;
    int newRecs;
    void Action(void);
public:
    cDisplayMenuItemCurrentRecordingView(cTemplateViewElement *tmplCurrent, const cRecording *recording, int level, int total, int newRecs);
    virtual ~cDisplayMenuItemCurrentRecordingView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

class cDisplayMenuItemCurrentPluginView: public cDisplayMenuItemCurrentView {
private:
    map<string,vector<map<string,string> > > loopTokens;
    void Action(void);
public:
    cDisplayMenuItemCurrentPluginView(cTemplateViewElement *tmplCurrent, map <string,string> &plugStringTokens, map <string,int> &plugIntTokens, map<string,vector<map<string,string> > > &pluginLoopTokens);
    virtual ~cDisplayMenuItemCurrentPluginView();
    void Prepare(void);
    void Render(void);
    void Clear(void);
};

#endif //__DISPLAYMENUITEMCURRENTVIEW_H
