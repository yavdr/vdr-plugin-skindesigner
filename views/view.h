#ifndef __VIEW_H
#define __VIEW_H

#include "string"
#include "map"
#include "../libcore/pixmapcontainer.h"
#include "../libtemplate/template.h"
#include "viewhelpers.h"
#include "animation.h"

using namespace std;

class cViewElement;

class cView : public cPixmapContainer {
private:
    void Init(void);
    void DoDrawDebugGrid(void);
    void DoFill(int num, cTemplateFunction *func);
    void DoDrawText(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawTextVertical(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawTextBox(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawFloatingTextBox(int num, cTemplateFunction *func);
    void DoDrawRectangle(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawEllipse(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawSlope(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DoDrawImage(int num, cTemplateFunction *func, int x0 = 0, int y0 = 0);
    void DrawAnimatedImage(int numPix, cTemplateFunction *func, cRect &pos, cImage *image);
    void DrawAnimatedText(int numPix, cTemplateFunction *func, cPoint &pos, string text, tColor col, string fontName, int fontSize);
    void DrawAnimatedOsdObject(int numPix, cTemplateFunction *func, cRect &pos, tColor col, int quadrant);
    cRect CalculateAnimationClip(int numPix, cRect &pos);
    void ActivateScrolling(void);
protected:
    cTemplateView *tmplView;
    cTemplateViewElement *tmplViewElement;
    cTemplateViewTab *tmplTab;
    //detached viewelements
    map < eViewElement, cViewElement* > detachedViewElements;
    //animated elements
    multimap < int, cAnimation* > animations;
    //scaling window
    cRect scalingWindow;
    bool tvScaled;
    bool viewInit;
    //do fadeout or shiftout only for views, not for childs
    bool fadeOut;
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
    int animCat;
    void DrawViewElement(eViewElement ve, map <string,string> *stringTokens = NULL, map <string,int> *intTokens = NULL, map < string, vector< map< string, string > > > *loopTokens = NULL);
    void ClearViewElement(eViewElement ve);
    void DestroyViewElement(eViewElement ve);
    void DestroyDetachedViewElement(eViewElement ve);
    void ClearAnimations(int cat);
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
    void DrawDebugGrid(void);
    virtual ~cView();
    virtual void Stop(void);
    void HideAnimations(void);
    void ShowAnimations(void);
};

class cViewElement : public cView {
private:
protected:
    bool init;
    eViewElement ve;
    int delay;
    stringmap stringTokens;
    intmap intTokens;
    bool (cViewHelpers::*SetTokens)(bool, stringmap&, intmap&);
    cViewHelpers *helper;
    void Action(void);
    void ClearTokens(void);
public:
    cViewElement(cTemplateViewElement *tmplViewElement);
    cViewElement(cTemplateViewElement *tmplViewElement, cViewHelpers *helper);
    virtual ~cViewElement();
    void SetCallback(eViewElement ve, bool (cViewHelpers::*SetTokens)(bool, stringmap&, intmap&)) { this->ve = ve; this->SetTokens = SetTokens; };
    virtual bool Render(void);
    void Clear(void);
    bool Starting(void) { return Running(); };
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