#define __STL_CONFIG_H
#include "displaypluginview.h"

cDisplayPluginView::cDisplayPluginView(cTemplateView *tmplView, bool isRootView) : cView(tmplView) {
    init = true;
    tabInit = true;
    tabScrolled = true;
    hidden = false;
    intTokens = NULL;
    stringTokens = NULL;
    loopTokens = NULL;
    currentTmplTab = NULL;
    tabView = NULL;
    if (isRootView)
        DeleteOsdOnExit();
    SetFadeTime(tmplView->GetNumericParameter(ptFadeTime));
}

cDisplayPluginView::~cDisplayPluginView() {
    CancelSave();
    FadeOut();
    if (tabView)
        delete tabView;
}

bool cDisplayPluginView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    return ok;
}

void cDisplayPluginView::Deactivate(bool hide) {
    Stop();
    if (!hide) {
        bool hideFromTemplate = tmplView->HideView();
        if (!hideFromTemplate)
            return;
        esyslog("skindesigner: hiding from template");
    }
    HidePixmaps();
    for (map< int, cViewGrid* >::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
        cViewGrid *viewGrid = it->second;
        viewGrid->Hide();
    }
    hidden = true;
}

void cDisplayPluginView::Activate(void) {
    if (tvScaled) {
        cDevice::PrimaryDevice()->ScaleVideo(scalingWindow);
    }
    if (hidden) {
        ShowPixmaps();
        for (map< int, cViewGrid* >::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
            cViewGrid *viewGrid = it->second;
            viewGrid->Show();
        }
    }
    Start();
}

void cDisplayPluginView::CleanViewElement(int id) {
    if (ViewElementScrolls((eViewElement)id)) {
        currentlyScrolling = false;
        if (Running())
            Stop();
        DestroyViewElement((eViewElement)id);
    } else {
        ClearViewElement((eViewElement)id);
    }
}

void cDisplayPluginView::DisplayViewElement(int id) {
    if (!intTokens || !stringTokens || !loopTokens)
        return;
    DrawViewElement((eViewElement)id, stringTokens, intTokens, loopTokens);
    if (!init && ViewElementScrolls((eViewElement)id))
        Start();
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

void cDisplayPluginView::SetTabIntTokens(map<string,int> *intTokens) {
    tabIntTokens = *intTokens;
}

void cDisplayPluginView::SetTabStringTokens(map<string,string> *stringTokens) {
    tabStringTokens = *stringTokens;
}

void cDisplayPluginView::SetTabLoopTokens(map<string,vector<map<string,string> > > *loopTokens) {
    tabLoopTokens = *loopTokens;
}

void cDisplayPluginView::SetTabs(void) {
    tmplView->InitViewTabIterator();
    cTemplateViewTab *tmplTab = NULL;
    while(tmplTab = tmplView->GetNextViewTab()) {
        tmplTab->ParseDynamicParameters(&tabIntTokens, true);
        tmplTab->ClearDynamicFunctionParameters();
        tmplTab->ParseDynamicFunctionParameters(&tabStringTokens, &tabIntTokens);
        if (tmplTab->DoExecute()) {
            activeTabs.push_back(tmplTab);
        }
    }
    atIt = activeTabs.begin();
}

void cDisplayPluginView::TabLeft(void) {
    if (activeTabs.size() > 1) {
        currentTmplTab = GetPrevTab();
        delete tabView;
        tabView = NULL;
        tabScrolled = true;
    } else {
        tabScrolled = tabView->KeyLeft();
    }
}

void cDisplayPluginView::TabRight(void) {
    if (activeTabs.size() > 1) {
        currentTmplTab = GetNextTab();
        delete tabView;
        tabView = NULL;
        tabScrolled = true;
    } else {
        tabScrolled = tabView->KeyRight();
    }
}

void cDisplayPluginView::TabUp(void) {
    tabScrolled = tabView->KeyUp();
}

void cDisplayPluginView::TabDown(void) {
    tabScrolled = tabView->KeyDown();
}

void cDisplayPluginView::DisplayTab(void) {
    if (tabInit) {
        currentTmplTab = *atIt;
        tabInit = false;
    }
    if (!tabView) {
        tabView = new cDisplayMenuTabView(currentTmplTab);
        tabView->SetTokens(&tabIntTokens, &tabStringTokens, &tabLoopTokens);
        tabView->CreateTab();
        tabView->Start();
        DrawTabLabels();
    }
    if (tabScrolled) {
        DrawScrollbar();
    }
}

cTemplateViewTab *cDisplayPluginView::GetPrevTab(void) {
    if (atIt == activeTabs.begin()) {
        atIt = activeTabs.end();
    }
    atIt--;
    return *atIt;
}

cTemplateViewTab *cDisplayPluginView::GetNextTab(void) {
    atIt++;
    if (atIt == activeTabs.end()) {
        atIt = activeTabs.begin();
    }
    return *atIt;
}

void cDisplayPluginView::DrawScrollbar(void) {
    map < string, string > scrollbarStringTokens;
    map < string, int > scrollbarIntTokens;

    int barTop = 0;
    int barHeight = 0;
    tabView->GetScrollbarPosition(barTop, barHeight);
    
    scrollbarIntTokens.insert(pair<string,int>("height", barHeight));
    scrollbarIntTokens.insert(pair<string,int>("offset", barTop));
    ClearViewElement((eViewElement)pveScrollbar);
    DrawViewElement((eViewElement)pveScrollbar, &scrollbarStringTokens, &scrollbarIntTokens);
}

void cDisplayPluginView::DrawTabLabels(void) {
    if (!ExecuteViewElement((eViewElement)pveTablabels)) {
        return;
    }
    map < string, string > labelStringTokens;
    map < string, int > labelIntTokens;
    map < string, vector< map< string, string > > > labelLoopTokens;

    string labelPrev = "";
    string labelPrevTemp = "";
    string labelCurrent = "";
    string labelNext = "";
    bool wasCurrent = false;
    vector< map< string, string > > tabLabels;
    for (list<cTemplateViewTab*>::iterator it = activeTabs.begin(); it != activeTabs.end(); it++) {
        cTemplateViewTab *tab = *it;
        map< string, string > tabLabel;
        tabLabel.insert(pair< string, string >("tabs[title]", tab->GetName()));
        if (wasCurrent) {
            labelNext = tab->GetName();
        }
        if (tab == currentTmplTab) {
            wasCurrent = true;
            labelCurrent = tab->GetName();
            labelPrev = labelPrevTemp;
            tabLabel.insert(pair< string, string >("tabs[current]", "1"));
        } else {
            wasCurrent = false;
            tabLabel.insert(pair< string, string >("tabs[current]", "0"));            
        }
        labelPrevTemp = tab->GetName();
        tabLabels.push_back(tabLabel);
    }
    if (labelNext.size() == 0 && activeTabs.size() > 0) {
        cTemplateViewTab *firstTab = activeTabs.front();
        labelNext = firstTab->GetName();
    }
    if (labelPrev.size() == 0 && activeTabs.size() > 0) {
        cTemplateViewTab *lastTab = activeTabs.back();
        labelPrev = lastTab->GetName();
    }
    labelStringTokens.insert(pair< string, string >("currenttab", labelCurrent));
    labelStringTokens.insert(pair< string, string >("nexttab", labelNext));
    labelStringTokens.insert(pair< string, string >("prevtab", labelPrev));
    labelLoopTokens.insert(pair< string, vector< map< string, string > > >("tabs", tabLabels));

    ClearViewElement((eViewElement)pveTablabels);
    DrawViewElement((eViewElement)pveTablabels, &labelStringTokens, &labelIntTokens, &labelLoopTokens);
}

void cDisplayPluginView::Action(void) {
    SetInitFinished();
    FadeIn();
    DoFlush();
    cView::Action();
}