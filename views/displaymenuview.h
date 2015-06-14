#ifndef __DISPLAYMENUVIEW_H
#define __DISPLAYMENUVIEW_H

#include "../libtemplate/template.h"
#include "viewhelpers.h"
#include "displaymenulistview.h"

#if APIVERSNUM < 20107
enum eMenuSortMode {
    msmUnknown = 0,
    msmNumber,
    msmName,
    msmTime,
    msmProvider
};
#endif

class cDisplayMenuView : public cView, public cViewHelpers {
protected:
    eMenuCategory cat;
    eMenuSortMode sortMode;
    eMenuSortMode sortModeLast;
    string menuTitle;
    string *buttonTexts;
    virtual void Action(void);
public:
    cDisplayMenuView(cTemplateView *tmplView, bool menuInit);
    virtual ~cDisplayMenuView();
    void SetMenuCat(eMenuCategory newCat) { cat = newCat; };
    void SetSortMode(eMenuSortMode sortMode) { this->sortMode = sortMode; };
    void SetTitle(const char *title) {menuTitle = title; };
    virtual void SetChannel(const cChannel *channel) {};
    virtual const cChannel *GetChannel(void) { return NULL; };
    virtual void ClearChannel(void) {};
    virtual void SetEpgSearchFavorite(void) {};
    virtual void ClearEpgSearchFavorite(void) {};
    void SetButtonTexts(string *buttonTexts) { this->buttonTexts = buttonTexts; };
    bool DrawBackground(void);
    virtual bool DrawHeader(void);
    bool DrawDateTime(bool forced, bool &implemented);
    bool DrawTime(bool forced, bool &implemented);
    bool DrawColorButtons(void);
    bool DrawMessage(eMessageType type, const char *text);
    void DrawScrollbar(int numMax, int numDisplayed, int offset);
    bool DrawSortMode(void);
    virtual void DrawStaticViewElements(void) {};
    virtual bool DrawDynamicViewElements(void) { return false; };
    bool BackgroundImplemented(void);
};

class cDisplayMenuMainView : public cDisplayMenuView {
private:
    bool initial;
    string currentRecording;
    void DrawTimers(void);
    void DrawLastRecordings(void);
    void DrawDiscUsage(void);
    bool DrawLoad(void);
    bool DrawMemory(void);
    bool DrawVdrStats(void);
    void DrawTemperatures(void);
    bool DrawDevices(void);
    void DrawCurrentSchedule(void);
    void DrawCurrentWeather(void);
    bool DrawCustomTokens(void);
public:
    cDisplayMenuMainView(cTemplateView *tmplView, bool menuInit, string currentRecording);
    virtual ~cDisplayMenuMainView();
    void DrawStaticViewElements(void);
    bool DrawDynamicViewElements(void);
};

class cDisplayMenuSchedulesView : public cDisplayMenuView {
private:
    const cChannel *channel;
    bool isEpgSearchFavoritesMenu;
public:
    cDisplayMenuSchedulesView(cTemplateView *tmplView, eMenuCategory menuCat, bool menuInit);
    virtual ~cDisplayMenuSchedulesView();
    void SetChannel(const cChannel *channel) { if (channel) this->channel = channel; };
    const cChannel *GetChannel(void) { return channel; };
    void ClearChannel(void) { channel = NULL; };
    void SetEpgSearchFavorite(void) { isEpgSearchFavoritesMenu = true; };
    void ClearEpgSearchFavorite(void) { isEpgSearchFavoritesMenu = false; };
    bool DrawHeader(void);
};

#endif //__DISPLAYMENUVIEW_H
