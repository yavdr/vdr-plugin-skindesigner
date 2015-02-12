#ifndef __DISPLAYPLUGINVIEW_H
#define __DISPLAYPLUGINVIEW_H

#include <vdr/thread.h>
#include "../libtemplate/template.h"
#include "view.h"
#include "viewgrid.h"

class cDisplayPluginView : public cView {
private:
    map<string,int> *intTokens;
    map<string,string> *stringTokens;
    map<string,vector<map<string,string> > > *loopTokens;
    map< int, cViewGrid* > viewGrids;
    virtual void Action(void);
public:
    cDisplayPluginView(cTemplateView *tmplView);
    virtual ~cDisplayPluginView();
    bool createOsd(void);
    void SetIntTokens(map<string,int> *intTokens) { this->intTokens = intTokens; };
    void SetStringTokens(map<string,string> *stringTokens) { this->stringTokens = stringTokens; };
    void SetLoopTokens(map<string,vector<map<string,string> > > *loopTokens) { this->loopTokens = loopTokens; };
    void DisplayViewElement(int id);
    void InitGrids(int viewGridID);
    void SetGrid(int viewGridID, long gridID, double x, double y, double width, double height, map<string,int> *intTokens, map<string,string> *stringTokens);
    void SetGridCurrent(int viewGridID, long gridID, bool current);
    void DeleteGrid(int viewGridID, long gridID);
    void DisplayGrids(int viewGridID);
    void ClearGrids(int viewGridID);
    void DoStart(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYPLUGINVIEW_H
