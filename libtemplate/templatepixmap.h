#ifndef __TEMPLATEPIXMAP_H
#define __TEMPLATEPIXMAP_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "globals.h"
#include "templateloopfunction.h"
#include "../views/viewhelpers.h"

using namespace std;

// --- cTemplatePixmapNode -------------------------------------------------------------
class cTemplatePixmapNode {
protected:
    bool isContainer;
    cGlobals *globals;
    cTemplateFunction *parameters;
    int containerX;
    int containerY;
    int containerWidth;
    int containerHeight;
public:
    cTemplatePixmapNode(void);
    virtual ~cTemplatePixmapNode(void);
    void SetParameters(vector<stringpair> &params);
    void SetContainer(int x, int y, int w, int h);
    bool IsContainer(void) { return isContainer; };
    bool DoExecute(void) { return parameters->DoExecute(); };
    bool DoDebug(void) { return parameters->DoDebug(); };
    virtual void SetGlobals(cGlobals *globals) { this->globals = globals; };
    virtual bool CalculateParameters(void) { return false; };
    virtual void SetWidth(int width) {};
    virtual void SetHeight(int height) {};
    virtual int NumPixmaps(void) { return 0; };
    virtual void Debug(void) {};
};

// --- cTemplatePixmap -------------------------------------------------------------
class cTemplatePixmapContainer;

class cTemplatePixmap : public cTemplatePixmapNode {
protected:
    cTemplatePixmapContainer *pixContainer;
    bool scrolling;
    bool background;
    vector<cTemplateFunction*> functions;
    vector<cTemplateFunction*>::iterator funcIt;
    //functions replacing {width(label)} and {height(label)} tokens
    bool ReplaceWidthFunctions(void);
    bool ReplaceHeightFunctions(map < string, vector< map< string, string > > > *loopTokens);
    //functions replacing {posx(label)} and {posy(label)} tokens
    bool ReplacePosXFunctions(void);
    bool ReplacePosYFunctions(void);
    //Get Scrolling Function
    cTemplateFunction *GetScrollFunction(void);
public:
    cTemplatePixmap(void);
    virtual ~cTemplatePixmap(void);
    //Setter Functions
    void SetPixmapContainer(cTemplatePixmapContainer *pixContainer) { this->pixContainer = pixContainer; };
    void SetScrolling(void) { scrolling = true; };
    void SetWidth(int width);
    void SetHeight(int height);
    void SetX(int x);
    void SetY(int y);
    void SetWidthPercent(double width);
    void SetHeightPercent(double height);
    void SetXPercent(double x);
    void SetYPercent(double y);
    void SetParameter(eParamType type, string value);
    void AddFunction(string name, vector<pair<string, string> > &params);
    void AddLoopFunction(cTemplateLoopFunction *lf);
    //PreCache Parameters
    bool CalculateParameters(void);
    //clear dynamically set function parameters
    void ClearDynamicFunctionParameters(void);
    //Clear dynamically set pixmap parameters
    void ClearDynamicParameters(void);
    //Parse pixmap parameters with dynamically set Tokens
    void ParseDynamicParameters(map <string,string> *stringTokens, map <string,int> *intTokens, bool initFuncs);
    //Parse all function parameters with dynamically set Tokens
    void ParseDynamicFunctionParameters(map <string,string> *stringTokens, map <string,int> *intTokens, map < string, vector< map< string, string > > > *loopTokens);
    //Calculate size of drawport in case area scrolls
    bool CalculateDrawPortSize(cSize &size, map < string, vector< map< string, string > > > *loopTokens = NULL);
    //Set max width for text in scrollarea
    void SetScrollingTextWidth(void);
    //Getter Functions
    int NumPixmaps(void) { return 1; };
    cRect GetPixmapSize(void);
    int GetNumericParameter(eParamType type);
    bool Scrolling(void) { return scrolling; };
    bool Ready(void);
    bool BackgroundArea(void) { return background; };
    bool ParameterSet(eParamType type);
    cTemplateFunction *GetFunction(string name);
    //Traverse Functions
    void InitFunctionIterator(void);
    cTemplateFunction *GetNextFunction(void);
    //Debug
    void Debug(void);
};

class cTemplatePixmapContainer : public cTemplatePixmapNode {
private:
    vector<cTemplatePixmap*> pixmaps;
    vector<cTemplatePixmap*>::iterator pixmapIterator;
public:
    cTemplatePixmapContainer(void);
    virtual ~cTemplatePixmapContainer(void);
    void SetGlobals(cGlobals *globals);
    void SetWidth(int width);
    void SetHeight(int height);
    void AddPixmap(cTemplatePixmap *pix);
    //PreCache Parameters
    bool CalculateParameters(void);
    void ParseDynamicParameters(map <string,string> *stringTokens, map <string,int> *intTokens);
    int NumPixmaps(void) { return pixmaps.size(); };
    void InitIterator(void);
    cTemplatePixmap *GetNextPixmap(void);
    cTemplateFunction *GetFunction(string name);
    void Debug(void);
};

#endif //__TEMPLATEPIXMAP_H