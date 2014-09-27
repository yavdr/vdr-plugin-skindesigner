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

using namespace std;

// --- cTemplatePixmap -------------------------------------------------------------

class cTemplatePixmap {
protected:
    bool scrolling;
    cTemplateFunction *parameters;
    vector<cTemplateFunction*> functions;
    vector<cTemplateFunction*>::iterator funcIt;
    int containerX;
    int containerY;
    int containerWidth;
    int containerHeight;
    cGlobals *globals;
    //functions replacing {width(label)} and {height(label)} tokens
    void ReplaceWidthFunctions(void);
    void ReplaceHeightFunctions(void);
    //functions replacing {posx(label)} and {posy(label)} tokens
    void ReplacePosXFunctions(void);
    void ReplacePosYFunctions(void);
    //Get Scrolling Function
    cTemplateFunction *GetScrollFunction(void);
public:
    cTemplatePixmap(void);
    virtual ~cTemplatePixmap(void);
    //Setter Functions
    void SetScrolling(void) { scrolling = true; };
    void SetParameters(vector<pair<string, string> > &params);
    void SetWidth(int width);
    void SetHeight(int height);
    void SetX(int x);
    void SetY(int y);
    void SetContainer(int x, int y, int w, int h);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void AddFunction(string name, vector<pair<string, string> > &params);
    void AddLoopFunction(cTemplateLoopFunction *lf);
    //PreCache Parameters
    bool CalculateParameters(void);
    //clear dynamically set function parameters
    void ClearDynamicFunctionParameters(void);
    //Clear dynamically set pixmap parameters
    void ClearDynamicParameters(void);
    //Parse pixmap parameters with dynamically set Tokens
    void ParseDynamicParameters(map <string,int> *intTokens, bool initFuncs);
    //Parse all function parameters with dynamically set Tokens
    void ParseDynamicFunctionParameters(map <string,string> *stringTokens, map <string,int> *intTokens);
    //Calculate size of drawport in case area scrolls
    bool CalculateDrawPortSize(cSize &size, map < string, vector< map< string, string > > > *loopTokens = NULL);
    //Set max width for text in scrollarea
    void SetScrollingTextWidth(void);
    //Getter Functions
    cRect GetPixmapSize(void);
    int GetNumericParameter(eParamType type);
    bool Scrolling(void) { return scrolling; };
    bool DoExecute(void) { return parameters->DoExecute(); };
    bool DoDebug(void) { return parameters->DoDebug(); };
    bool Ready(void);
    //Traverse Functions
    void InitIterator(void);
    cTemplateFunction *GetNextFunction(void);
    //Debug
    void Debug(void);
};

#endif //__TEMPLATEPIXMAP_H