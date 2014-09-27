#ifndef __TEMPLATEVIEWTAB_H
#define __TEMPLATEVIEWTAB_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "templatepixmap.h"

using namespace std;

// --- cTemplateViewTab -------------------------------------------------------------

class cTemplateViewTab : public cTemplatePixmap {
private:
    int scrollStep;
public:
    cTemplateViewTab(void);
    ~cTemplateViewTab(void);
    int GetScrollStep(void);
    string GetName(void);
    void SetName(string trans);
    void Debug(void);
};

#endif //__TEMPLATEVIEWTAB_H