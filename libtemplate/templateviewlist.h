#ifndef __TEMPLATEVIEWLIST_H
#define __TEMPLATEVIEWLIST_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "templateviewelement.h"

using namespace std;

// --- cTemplateViewList -------------------------------------------------------------

enum eViewList {
    vlUndefined,
    vlMenuItem
};

class cTemplateViewList : public cTemplateViewElement {
private:
    cTemplateViewElement *listElement;
    cTemplateViewElement *currentElement;
public:
    cTemplateViewList(void);
    ~cTemplateViewList(void);
    void SetGlobals(cGlobals *globals);
    void AddListElement(cTemplateViewElement *listElement) { this->listElement = listElement; };
    void AddCurrentElement(cTemplateViewElement *currentElement) { this->currentElement = currentElement; };
    bool CalculateListParameters(void);
    bool CalculateListParameters(map < string, int > *intTokens);
    cTemplateViewElement *GetListElement(void) { return listElement; };
    cTemplateViewElement *GetListElementCurrent(void) { return currentElement; };
    eOrientation GetOrientation(void);
    int GetAverageFontWidth(void);
    cFont *GetTextAreaFont(void);
    int GetMenuItemWidth(void);
    int GetNumPixmaps(void);
    void Debug(void);
};

#endif //__TEMPLATEVIEWLIST_H