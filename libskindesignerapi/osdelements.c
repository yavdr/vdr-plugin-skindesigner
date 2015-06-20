#include "osdelements.h"

/**********************************************************************
* cOsdElement
**********************************************************************/
skindesignerapi::cOsdElement::cOsdElement(skindesignerapi::ISkinDisplayPlugin *view) {
    this->view = view;
}

skindesignerapi::cOsdElement::~cOsdElement() {
}

void skindesignerapi::cOsdElement::ClearTokens(void) {
    stringTokens.clear();
    intTokens.clear();
    loopTokens.clear();
}

void skindesignerapi::cOsdElement::AddStringToken(string key, string value) {
    stringTokens.insert(pair<string,string>(key, value));
}

void skindesignerapi::cOsdElement::AddIntToken(string key, int value) {
    intTokens.insert(pair<string,int>(key, value));
}

void skindesignerapi::cOsdElement::AddLoopToken(string loopName, map<string, string> &tokens) {
    map<string, vector<map<string, string> > >::iterator hitLoop = loopTokens.find(loopName);
    if (hitLoop == loopTokens.end()) {
        vector<map<string, string> > tokenVector;
        tokenVector.push_back(tokens);
        loopTokens.insert(pair<string, vector<map<string, string> > >(loopName, tokenVector));
    } else {
        vector<map<string, string> > *tokenVector = &hitLoop->second;
        tokenVector->push_back(tokens);
    }
}

bool skindesignerapi::cOsdElement::ChannelLogoExists(string channelId) {
    return view->ChannelLogoExists(channelId);
}

string skindesignerapi::cOsdElement::GetEpgImagePath(void) {
    return view->GetEpgImagePath();    
}


/**********************************************************************
* cViewElement
**********************************************************************/
skindesignerapi::cViewElement::cViewElement(skindesignerapi::ISkinDisplayPlugin *view, int viewElementID) : cOsdElement(view) {
    this->viewElementID = viewElementID;
}

skindesignerapi::cViewElement::~cViewElement() {
}

void skindesignerapi::cViewElement::Clear(void) {
    if (!view)
        return;
    view->ClearViewElement(viewElementID);
}

void skindesignerapi::cViewElement::Display(void) {
    if (!view)
        return;
    view->SetViewElementIntTokens(&intTokens);
    view->SetViewElementStringTokens(&stringTokens);
    view->SetViewElementLoopTokens(&loopTokens);
    view->DisplayViewElement(viewElementID);
}

/**********************************************************************
* cViewGrid
**********************************************************************/
skindesignerapi::cViewGrid::cViewGrid(skindesignerapi::ISkinDisplayPlugin *view, int viewGridID) : cOsdElement(view) {
    this->viewGridID = viewGridID;
}

skindesignerapi::cViewGrid::~cViewGrid() {
    if (!view)
        return;
    view->ClearGrids(viewGridID);    
}

void skindesignerapi::cViewGrid::SetGrid(long gridID, double x, double y, double width, double height) {
    if (!view)
        return;
    view->SetGrid(viewGridID, gridID, x, y, width, height, &intTokens, &stringTokens);
}

void skindesignerapi::cViewGrid::SetCurrent(long gridID, bool current) {
    if (!view)
        return;
    view->SetGridCurrent(viewGridID, gridID, current);
}

void skindesignerapi::cViewGrid::MoveGrid(long gridID, double x, double y, double width, double height) {
    if (!view)
        return;
    view->SetGrid(viewGridID, gridID, x, y, width, height, NULL, NULL);
}

void skindesignerapi::cViewGrid::Delete(long gridID) {
    if (!view)
        return;
    view->DeleteGrid(viewGridID, gridID);
}

void skindesignerapi::cViewGrid::Clear(void) {
    if (!view)
        return;
    view->ClearGrids(viewGridID);    
}

void skindesignerapi::cViewGrid::Display(void) {
    if (!view)
        return;
    view->DisplayGrids(viewGridID);
}

/**********************************************************************
* cViewTab
**********************************************************************/
skindesignerapi::cViewTab::cViewTab(skindesignerapi::ISkinDisplayPlugin *view) : cOsdElement(view) {
}

skindesignerapi::cViewTab::~cViewTab() {
}

void skindesignerapi::cViewTab::Init(void) {
    view->SetTabIntTokens(&intTokens);
    view->SetTabStringTokens(&stringTokens);
    view->SetTabLoopTokens(&loopTokens);
    view->SetTabs();    
}

void skindesignerapi::cViewTab::Left(void) {
    view->TabLeft();
}

void skindesignerapi::cViewTab::Right(void) {
    view->TabRight();
}

void skindesignerapi::cViewTab::Up(void) {
    view->TabUp();
}

void skindesignerapi::cViewTab::Down(void) {
    view->TabDown();
}

void skindesignerapi::cViewTab::Display(void) {
    if (!view)
        return;
    view->DisplayTabs();
}

/**********************************************************************
* cOsdView
**********************************************************************/
skindesignerapi::cOsdView::cOsdView(skindesignerapi::ISkinDisplayPlugin *displayPlugin) {
    this->displayPlugin = displayPlugin;
}

skindesignerapi::cOsdView::~cOsdView() {
    delete displayPlugin;
}

void skindesignerapi::cOsdView::Deactivate(bool hide) {
    if (!displayPlugin)
        return;
    displayPlugin->Deactivate(hide);
}

void skindesignerapi::cOsdView::Activate(void) {
    if (!displayPlugin)
        return;
    displayPlugin->Activate();
}

skindesignerapi::cViewElement *skindesignerapi::cOsdView::GetViewElement(int viewElementID) {
    if (!displayPlugin)
        return NULL;
    return new cViewElement(displayPlugin, viewElementID);
}

skindesignerapi::cViewGrid *skindesignerapi::cOsdView::GetViewGrid(int viewGridID) {
    if (!displayPlugin)
        return NULL;
    displayPlugin->InitGrids(viewGridID);
    return new cViewGrid(displayPlugin, viewGridID);
}

skindesignerapi::cViewTab *skindesignerapi::cOsdView::GetViewTabs(void) {
    if (!displayPlugin)
        return NULL;
    return new cViewTab(displayPlugin);
}

void skindesignerapi::cOsdView::Display(void) {
    if (!displayPlugin)
        return;
    displayPlugin->Flush();   
}
