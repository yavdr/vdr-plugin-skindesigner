#include "displaymenutabview.h"


cDisplayMenuTabView::cDisplayMenuTabView(cTemplateViewTab *tmplTab) : cView(tmplTab) {
}

cDisplayMenuTabView::~cDisplayMenuTabView() {
}

void cDisplayMenuTabView::SetTokens(map < string, int > *intTokens, map < string, string > *stringTokens, map < string, vector< map< string, string > > > *loopTokens) {
    this->intTokens = intTokens;
    this->stringTokens = stringTokens;
    this->loopTokens = loopTokens;
}

void cDisplayMenuTabView::Clear(void) {
    Fill(0, clrTransparent);
}

void cDisplayMenuTabView::CreateTab(void) {
    //Create Pixmap
    if (!PixmapExists(0)) {
        cSize drawportSize;
        scrolling = tmplTab->CalculateDrawPortSize(drawportSize, loopTokens);
        if (scrolling) {
            CreateScrollingPixmap(0, tmplTab, drawportSize);
            scrollingPix = 0;
            scrollOrientation = orVertical; 
            scrollMode = smNone;
        } else {
            CreateViewPixmap(0, tmplTab);              
        }
    }    
}


void cDisplayMenuTabView::Render(void) {
    if (tmplTab->DoDebug()) {
        tmplTab->Debug();
    }
    //Draw Tab, flushing every loop
    DrawPixmap(0, tmplTab, loopTokens, true);
}

bool cDisplayMenuTabView::KeyUp(void) { 
    if (!scrolling)
        return false;
    int scrollStep = tmplTab->GetScrollStep();
    int aktHeight = DrawportY(0);
    if (aktHeight >= 0) {
        return false;
    }
    int newY = aktHeight + scrollStep;
    if (newY > 0)
        newY = 0;
    SetDrawPortPoint(0, cPoint(0, newY));
    return true;
}

bool cDisplayMenuTabView::KeyDown(void) {
    if (!scrolling)
        return false;

    int scrollStep = tmplTab->GetScrollStep();
    int aktHeight = DrawportY(0);
    int totalHeight = DrawportHeight(0);
    int screenHeight = Height(0);
    
    if (totalHeight - ((-1)*aktHeight) == screenHeight) {
        return false;
    } 
    int newY = aktHeight - scrollStep;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    SetDrawPortPoint(0, cPoint(0, newY));
    return true;
}

bool cDisplayMenuTabView::KeyLeft(void) {
    if (!scrolling)
        return false;
    if (!PixmapExists(0))
        return false;
    int aktHeight = DrawportY(0);
    int screenHeight = Height(0);
    int newY = aktHeight + screenHeight;
    if (newY > 0)
        newY = 0;
    SetDrawPortPoint(0, cPoint(0, newY));
    return true;
}

bool cDisplayMenuTabView::KeyRight(void) {
    if (!scrolling)
        return false;
    if (!PixmapExists(0))
        return false;
    int aktHeight = DrawportY(0);
    int screenHeight = Height(0);
    int totalHeight = DrawportHeight(0);
    int newY = aktHeight - screenHeight;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    SetDrawPortPoint(0, cPoint(0, newY));
    return true;
}

void cDisplayMenuTabView::GetScrollbarPosition(int &barTop, int &barHeight) {
    int y = (-1)*DrawportY(0);
    int totalHeight = DrawportHeight(0);
    int screenHeight = Height(0);
    if (totalHeight == 0)
        return;
    if (totalHeight <= screenHeight)
        barHeight = 1000;
    else {
        barHeight = (double)screenHeight / (double) totalHeight * 1000; 
    }
    barTop = (double)y / (double) totalHeight * 1000;
}

void cDisplayMenuTabView::Action(void) {
    Render();
    DoFlush();
}