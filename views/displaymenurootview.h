#ifndef __DISPLAYMENUROOTVIEW_H
#define __DISPLAYMENUROOTVIEW_H

#include "../libtemplate/template.h"
#include "displaymenuview.h"
#include "displaymenulistview.h"
#include "displaymenudetailview.h"

class cDisplayMenuRootView : public cView {
private:
    eSubView viewType;
    cTemplateView *subView;
    bool subViewAvailable;
    cDisplayMenuView *view;
    cDisplayMenuListView *listView;
    cDisplayMenuDetailView *detailView;
    string menuTitle;
    string buttonTexts[4];
    bool defaultBackgroundDrawn;
    bool defaultHeaderDrawn;
    bool defaultButtonsDrawn;
    bool defaultDateTimeDrawn;
    bool defaultMessageDrawn;
    void DrawBackground(void);
    void DrawHeader(void);
    void DrawDateTime(void);
    void DrawColorButtons(void);
    void DrawMessage(eMessageType type, const char *text);
    void ClearRootView(void);
    virtual void Action(void);
public:
    cDisplayMenuRootView(cTemplateView *rootView);
    virtual ~cDisplayMenuRootView();
    bool createOsd(void);
    void SetMenu(eMenuCategory menuCat, bool menuInit);
    void SetTitle(const char *title);
    void SetChannel(const cChannel *channel) { view->SetChannel(channel); };
    void SetButtonTexts(const char *Red, const char *Green, const char *Yellow, const char *Blue);
    void SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5);
    void SetMessage(eMessageType type, const char *text);
    void SetDetailedViewEvent(const cEvent *event);
    void SetDetailedViewRecording(const cRecording *recording);
    void SetDetailedViewText(const char *text);
    void KeyInput(bool up, bool page);
    void Clear(void);
    int GetMaxItems(void);
    int GetListViewWidth(void);
    int GetTextAreaWidth(void);
    bool SubViewAvailable(void) { return subViewAvailable; };
    cDisplayMenuListView *GetListView(void) { return listView; };
    void Render(void);
    void RenderMenuItems(void);
    void RenderDetailView(void);
    void RenderMenuScrollBar(int Total, int Offset);
    bool RenderDynamicElements(void);
};
#endif //__DISPLAYMENUROOTVIEW_H
