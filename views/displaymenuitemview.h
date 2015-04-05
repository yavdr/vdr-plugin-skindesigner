#ifndef __DISPLAYMENUITEMVIEW_H
#define __DISPLAYMENUITEMVIEW_H

#include "../libtemplate/template.h"
#include "view.h"
#include "displaymenuitemcurrentview.h"

class cDisplayMenuItemView : public cViewListItem {
private:
protected:
    bool itemInit;
    bool dirty;
    bool current;
    bool selectable;
    int num;
    cTemplateViewList *tmplList;
    map < string, string > stringTokens;
    map < string, int > intTokens;
    cDisplayMenuItemCurrentView *currentView;
    virtual void Action(void);
public:
    cDisplayMenuItemView(cTemplateViewList *tmplList, bool current, bool selectable);
    virtual ~cDisplayMenuItemView();
    void SetCurrent(bool cur);
    void SetNumber(int n) { num = n; };
    void ArrangeContainer(void);
    bool Current(void) { return current; };
    void PrepareScrolling(void);
    void EndScrolling(void);
    virtual void SetTokens(void) {};
    virtual void Prepare(void) {};
    virtual void Render(void) {};
    virtual void Clear(void);
    bool Dirty(void) { return dirty; };
    void Stop(void);
    virtual void Debug(void);
};

class cDisplayMenuItemDefaultView: public cDisplayMenuItemView {
private:
    int *tabs;
    int *tabWidths;
    string *tabTexts;
    int maxTabs;
    string menuCategory;
public:
    cDisplayMenuItemDefaultView(cTemplateViewList *tmplList, string *tabTexts, int *tabs, int *tabWidths, bool current, bool selectable);
    virtual ~cDisplayMenuItemDefaultView();
    void SetTabTexts(string *tabTexts);
    void SetTokens(void);
    void SetMenuCategory(string cat) { menuCategory = cat; };
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemMainView: public cDisplayMenuItemView {
private:
    bool isPlugin;
    string plugName;
    string text;
    string number;
    string label;
    string icon;
    void SplitMenuText(void);
    void CheckPlugins(void);
public:
    cDisplayMenuItemMainView(cTemplateViewList *tmplList, string itemText, bool current, bool selectable);
    virtual ~cDisplayMenuItemMainView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    string GetPluginName(void);
    void Debug(void);
};

class cDisplayMenuItemSchedulesView: public cDisplayMenuItemView {
private:
    const cEvent *event;
    const cChannel *channel;
    eTimerMatch timerMatch;
    eMenuCategory cat;
    bool isEpgSearchFav;
    string ParseSeparator(string sep);
public:
    cDisplayMenuItemSchedulesView(cTemplateViewList *tmplList, const cEvent *event, const cChannel *channel, eTimerMatch timerMatch, 
                                  eMenuCategory cat, bool isEpgSearchFav, bool current, bool selectable);
    virtual ~cDisplayMenuItemSchedulesView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemChannelsView: public cDisplayMenuItemView {
private:
    const cChannel *channel;
    bool withProvider;
public:
    cDisplayMenuItemChannelsView(cTemplateViewList *tmplList, const cChannel *channel, bool withProvider, bool current, bool selectable);
    virtual ~cDisplayMenuItemChannelsView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemTimersView: public cDisplayMenuItemView {
private:
    const cTimer *timer;
public:
    cDisplayMenuItemTimersView(cTemplateViewList *tmplList, const cTimer *timer, bool current, bool selectable);
    virtual ~cDisplayMenuItemTimersView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemRecordingView: public cDisplayMenuItemView {
private:
    const cRecording *recording;
    int level;
    int total;
    int newRecs;
public:
    cDisplayMenuItemRecordingView(cTemplateViewList *tmplList, const cRecording *recording, int level, int total, int newRecs, bool current, bool selectable);
    virtual ~cDisplayMenuItemRecordingView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemPluginView: public cDisplayMenuItemView {
private:
    map<string,vector<map<string,string> > > loopTokens;
public:
    cDisplayMenuItemPluginView(cTemplateViewList *tmplList, map<string,string> *plugStringTokens, map<string,int> *plugIntTokens, map<string,vector<map<string,string> > > *pluginLoopTokens, int index, bool current, bool selectable);
    virtual ~cDisplayMenuItemPluginView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

class cDisplayMenuItemTrackView: public cDisplayMenuItemView {
private:
    const char *title;
    int numTracks;
public:
    cDisplayMenuItemTrackView(cTemplateViewList *tmplList, const char *title, bool current, bool selectable, int numTracks);
    virtual ~cDisplayMenuItemTrackView();
    void SetTokens(void);
    void Prepare(void);
    void Render(void);
    void Debug(void);
};

#endif //__DISPLAYMENUITEMVIEW_H
