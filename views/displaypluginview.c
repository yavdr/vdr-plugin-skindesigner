#define __STL_CONFIG_H
#include "displaypluginview.h"

cDisplayPluginView::cDisplayPluginView(cTemplateView *tmplView) : cView(tmplView) {
    intTokens = NULL;
    stringTokens = NULL;
    loopTokens = NULL;
    DeleteOsdOnExit();
    SetFadeTime(tmplView->GetNumericParameter(ptFadeTime));
}

cDisplayPluginView::~cDisplayPluginView() {
    CancelSave();
    FadeOut();
}

bool cDisplayPluginView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    return ok;
}

void cDisplayPluginView::DisplayViewElement(int id) {
    if (!intTokens || !stringTokens || !loopTokens)
        return;
    DrawViewElement((eViewElement)id, stringTokens, intTokens, loopTokens);
}

void cDisplayPluginView::InitGrids(int viewGridID) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit != viewGrids.end()) {
        delete hit->second;
        viewGrids.erase(hit);
    }
    cTemplateViewGrid *tmplGrid = tmplView->GetViewGrid(viewGridID);
    cViewGrid *grid = new cViewGrid(tmplGrid);
    viewGrids.insert(pair< int, cViewGrid* >(viewGridID, grid));
}

void cDisplayPluginView::SetGrid(int viewGridID, long gridID, 
                                 double x, double y, double width, double height, 
                                 map<string,int> *intTokens, map<string,string> *stringTokens) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit != viewGrids.end())
        (hit->second)->SetGrid(gridID, x, y, width, height, intTokens, stringTokens);
}

void cDisplayPluginView::SetGridCurrent(int viewGridID, long gridID, bool current) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit != viewGrids.end())
        (hit->second)->SetCurrent(gridID, current);
}

void cDisplayPluginView::DeleteGrid(int viewGridID, long gridID) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit == viewGrids.end())
        return;
    (hit->second)->Delete(gridID);    
}

void cDisplayPluginView::DisplayGrids(int viewGridID) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit == viewGrids.end())
        return;
    (hit->second)->Render();
}

void cDisplayPluginView::ClearGrids(int viewGridID) {
    map< int, cViewGrid* >::iterator hit = viewGrids.find(viewGridID);
    if (hit == viewGrids.end())
        return;
    (hit->second)->Clear();
}

void cDisplayPluginView::Action(void) {
    SetInitFinished();
    FadeIn();
    DoFlush();
    cView::Action();
}
