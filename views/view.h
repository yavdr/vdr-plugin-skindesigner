#ifndef __VIEW_H
#define __VIEW_H

#include "string"
#include "map"
#include "../libcore/pixmapcontainer.h"
#include "../libtemplate/template.h"

using namespace std;

class cViewElement;

class cView : public cPixmapContainer {
private:
    void Init(void);
    void DoFill(int num, cTemplateFunction *func);
    void DoDrawText(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawTextVertical(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawTextBox(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawFloatingTextBox(int num, cTemplateFunction *func);
    void DoDrawRectangle(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawEllipse(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawSlope(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawImage(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void ActivateScrolling(void);
protected:
    cTemplateView *tmplView;
    cTemplateViewElement *tmplViewElement;
    cTemplateViewTab *tmplTab;
    //detached viewelements
    map < eViewElement, cViewElement* > detachedViewElements;
    //scaling window
    cRect scalingWindow;
    bool tvScaled;
    bool viewInit;
    //true if view is scrollable in general
    bool scrolling;
    //true if view is actually starting scrolling
    bool currentlyScrolling;
    eViewElement veScroll;
    int scrollingPix;
    int scrollOrientation;
    int scrollDelay;
    int scrollMode;
    int scrollSpeed;
    void DrawViewElement(eViewElement ve, map <string,string> *stringTokens = NULL, map <string,int> *intTokens = NULL, map < string, vector< map< string, string > > > *loopTokens = NULL);
    void ClearViewElement(eViewElement ve);
    void DestroyViewElement(eViewElement ve);
    bool ExecuteViewElement(eViewElement ve);
    bool DetachViewElement(eViewElement ve);
    bool ViewElementScrolls(eViewElement ve);
    cViewElement *GetViewElement(eViewElement ve);
    void AddViewElement(eViewElement ve, cViewElement *viewElement);
    void CreateViewPixmap(int num, cTemplatePixmap *pix, cRect *size = NULL);
    void CreateScrollingPixmap(int num, cTemplatePixmap *pix, cSize &drawportSize);
    void DrawPixmap(int num, cTemplatePixmap *pix, map < string, vector< map< string, string > > > *loopTokens = NULL, bool flushPerLoop = false);
    void DrawLoop(int numPixmap, cTemplateFunction *func, map < string, vector< map< string, string > > > *loopTokens);
    void DebugTokens(string viewElement, map<string,string> *stringTokens, map<string,int> *intTokens, map < string, vector< map< string, string > > > *loopTokens = NULL);
    virtual void Action(void);
public:
    cView(cTemplateView *tmplView);
    cView(cTemplateViewElement *tmplViewElement);
    cView(cTemplateViewTab *tmplTab);
    virtual ~cView();
    virtual void Stop(void);
};

class cViewElement : public cView {
private:
protected:
    int delay;
    map < string, string > stringTokens;
    map < string, int > intTokens;
    void Action(void);
    void Draw(map < string, vector< map< string, string > > > *loopTokens = NULL);
public:
    cViewElement(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElement();
    virtual void Render(void) {};
    void Clear(void);
};

class cViewListItem : public cView {
protected:
    int pos;
    int numTotal; 
    cRect container;
    int align;
    int listOrientation;
    void SetListElementPosition(cTemplatePixmap *pix);
public:
    cViewListItem(cTemplateViewElement *tmplItem);
    virtual ~cViewListItem();
    cRect DrawListItem(map <string,string> *stringTokens, map <string,int> *intTokens);
    void ClearListItem(void);
};

class cGrid : public cView {
protected:
    bool dirty;
    bool moved;
    bool resized;
    bool current;
    double x;
    double y;
    double width;
    double height;
    map <string,string> stringTokens;
    map <string,int> intTokens;
    void PositionPixmap(cTemplatePixmap *pix);
public:
    cGrid(cTemplateViewElement *tmplGrid);
    virtual ~cGrid();
    bool Dirty(void) { return dirty; };
    bool Moved(void) { return moved; };
    bool Resized(void) { return resized; };
    void Set(double x, double y, double width, double height, map <string,int> *intTokens, map <string,string> *stringTokens);
    void SetCurrent(bool current);
    void Move(void);
    void Draw(void);
    void Clear(void);
    void DeletePixmaps(void);
};

#endif //__VIEW_H