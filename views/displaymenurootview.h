#ifndef __DISPLAYMENUROOTVIEW_H
#define __DISPLAYMENUROOTVIEW_H

#include "../libtemplate/template.h"
#include "displaymenuview.h"
#include "displaymenulistview.h"
#include "displaymenudetailview.h"

enum ePluginMenuType {
    mtList,
    mtText,
    mtUnknown
};

class cDisplayMenuRootView : public cView, public cViewHelpers  {
private:
    eMenuCategory cat;
    string selectedPluginMainMenu;
    eMenuSortMode sortMode;
    eMenuSortMode sortModeLast;
    eSubView viewType;
    cTemplateView *subView;
    bool subViewAvailable;
    bool subViewInit;
    string pluginName;
    int pluginMenu;
    ePluginMenuType pluginMenuType;
    bool pluginMenuChanged;
    cDisplayMenuView *view;
    cDisplayMenuListView *listView;
    cDisplayMenuDetailView *detailView;
    string currentRecording;
    string menuTitle;
    string buttonTexts[4];
    bool defaultBackgroundDrawn;
    bool defaultHeaderDrawn;
    bool defaultButtonsDrawn;
    bool defaultDateTimeDrawn;
    bool defaultTimeDrawn;
    bool defaultMessageDrawn;
    bool defaultSortmodeDrawn;
    void DrawBackground(void);
    void DrawHeader(void);
    void DrawDateTime(bool forced);
    bool DrawTime(bool forced);
    void DrawSortMode(void);
    void DrawColorButtons(void);
    void DrawMessage(eMessageType type, const char *text);
    void ClearRootView(void);
    virtual void Action(void);
public:
    cDisplayMenuRootView(cTemplateView *rootView);
    virtual ~cDisplayMenuRootView();
    bool createOsd(void);
    void SetMenu(eMenuCategory menuCat, bool menuInit);
    void SetSelectedPluginMainMenu(string name) { selectedPluginMainMenu = name; };
    void SetSortMode(eMenuSortMode sortMode);
    eMenuOrientation MenuOrientation(void);
    void SetCurrentRecording(string rec) { currentRecording = rec; };
    void CorrectDefaultMenu(void);
    void SetPluginMenu(string name, int menu, int type);
    void SetTitle(const char *title);
    void SetChannel(const cChannel *channel) { view->SetChannel(channel); };
    const cChannel *GetChannel(void) { return view->GetChannel(); };
    void SetEpgSearchFavorite(void) { if (view) view->SetEpgSearchFavorite(); };
    void SetButtonTexts(const char *Red, const char *Green, const char *Yellow, const char *Blue);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetMessage(eMessageType type, const char *text);
    void SetDetailedViewEvent(const cEvent *event);
    void SetDetailedViewRecording(const cRecording *recording);
    void SetDetailedViewText(const char *text);
    bool SetDetailedViewPlugin(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens);
    void KeyInput(bool up, bool page);
    void Clear(void);
    int GetMaxItems(void);
    int GetListViewWidth(void);
    int GetTextAreaWidth(void);
    cFont *GetTextAreaFont(void);
    bool SubViewAvailable(void) { return subViewAvailable; };
    cDisplayMenuListView *GetListView(void) { return listView; };
    void Render(void);
    void RenderMenuItems(void);
    void RenderDetailView(void);
    void RenderMenuScrollBar(int Total, int Offset);
    bool RenderDynamicElements(void);
};
#endif //__DISPLAYMENUROOTVIEW_H
