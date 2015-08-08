#ifndef __DISPLAYMENULISTVIEW_H
#define __DISPLAYMENULISTVIEW_H

#include "../libtemplate/template.h"
#include "view.h"
#include "displaymenuitemview.h"

class cDisplayMenuListView {
private:
    cMutex mutex;
    cTemplateViewList *tmplList;
    eMenuCategory cat;
    string currentPlug;
    int itemCount;
    cDisplayMenuItemView **menuItems;
    int *tabs;
    int *tabWidths;
    bool oneColumn;
    string GetDefaultMenuCategory(void);
public:
    cDisplayMenuListView(cTemplateViewList *tmplList, int count, eMenuCategory cat = mcUnknown, string currentPlug = "");
    virtual ~cDisplayMenuListView();
    void Lock(void) { mutex.Lock(); };
    void Unlock(void) { mutex.Unlock(); };
    void Clear(void);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    int GetMaxItems(void) { return itemCount; };
    int GetListWidth(void);
    eMenuOrientation MenuOrientation(void);
    void AddDefaultMenuItem(int index, string *tabTexts, bool current, bool selectable);
    string AddMainMenuItem(int index, const char *itemText, bool current, bool selectable);
    void AddSetupMenuItem(int index, const char *itemText, bool current, bool selectable);
    void AddSchedulesMenuItem(int index, const cEvent *event, const cChannel *channel, eTimerMatch timerMatch, eMenuCategory cat, bool isEpgSearchFav, bool current, bool selectable, string dayseparator);
    void AddChannelsMenuItem(int index, const cChannel *channel, bool withProvider, bool current, bool selectable);
    void AddTimersMenuItem(int index, const cTimer *timer, bool current, bool selectable);
    void AddRecordingMenuItem(int index, const cRecording *recording, int level, int total, int isNew, bool current, bool selectable);
    void AddPluginMenuItem(map<string,string> *stringTokens, map<string,int> *intTokens, map<string, vector<map<string,string> > > *loopTokens, int index, bool current, bool selectable);
    void AddTracksMenuItem(int index, const char *title, bool current, bool selectable);
    void Render(void);
    void Debug(void);
};

#endif //__DISPLAYMENULISTVIEW_H
