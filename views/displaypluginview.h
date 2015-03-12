#ifndef __DISPLAYPLUGINVIEW_H
#define __DISPLAYPLUGINVIEW_H

#include <list>
#include <vdr/thread.h>
#include "../libtemplate/template.h"
#include "view.h"
#include "viewgrid.h"
#include "displaymenutabview.h"

class cDisplayPluginView : public cView {
private:
    bool init;
    bool tabInit;
    bool tabScrolled;
    bool hidden;
    map<string,int> *intTokens;
    map<string,string> *stringTokens;
    map<string,vector<map<string,string> > > *loopTokens;
    map< int, cViewGrid* > viewGrids;
    map<string,int> tabIntTokens;
    map<string,string> tabStringTokens;
    map<string,vector<map<string,string> > > tabLoopTokens;
    cTemplateViewTab *currentTmplTab;
    list<cTemplateViewTab*> activeTabs;
    list<cTemplateViewTab*>::iterator atIt;
    cDisplayMenuTabView *tabView;
    cTemplateViewTab *GetPrevTab(void);
    cTemplateViewTab *GetNextTab(void);
    void DrawScrollbar(void);
    void DrawTabLabels(void);
    virtual void Action(void);
public:
    cDisplayPluginView(cTemplateView *tmplView, bool isRootView);
    virtual ~cDisplayPluginView();
    bool createOsd(void);
    void Deactivate(bool hide);
    void Activate(void);
    void SetIntTokens(map<string,int> *intTokens) { this->intTokens = intTokens; };
    void SetStringTokens(map<string,string> *stringTokens) { this->stringTokens = stringTokens; };
    void SetLoopTokens(map<string,vector<map<string,string> > > *loopTokens) { this->loopTokens = loopTokens; };
    void CleanViewElement(int id);
    void DisplayViewElement(int id);
    void InitGrids(int viewGridID);
    void SetGrid(int viewGridID, long gridID, double x, double y, double width, double height, map<string,int> *intTokens, map<string,string> *stringTokens);
    void SetGridCurrent(int viewGridID, long gridID, bool current);
    void DeleteGrid(int viewGridID, long gridID);
    void DisplayGrids(int viewGridID);
    void ClearGrids(int viewGridID);
    void SetTabIntTokens(map<string,int> *intTokens);
    void SetTabStringTokens(map<string,string> *stringTokens);
    void SetTabLoopTokens(map<string,vector<map<string,string> > > *loopTokens);
    void SetTabs(void);
    void TabLeft(void);
    void TabRight(void);
    void TabUp(void);
    void TabDown(void);
    void DisplayTab(void);
    void DoStart(void) { init = false; Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYPLUGINVIEW_H
