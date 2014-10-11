#ifndef __XMLGLOBALS_H
#define __XMLGLOBALS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <vdr/plugin.h>

using namespace std;

typedef uint32_t tColor;

// --- cGlobals -------------------------------------------------------------

class cGlobals {
private:
    string language;
    string DoTranslate(string token);
public:
    cGlobals(void);
    virtual ~cGlobals(void) {};
    map <string, tColor> colors;
    map <string, int> intVars;
    map <string, double> doubleVars;
    map <string, string> stringVars;
    map <string, string> fonts;
    map <string, map< string, string > > translations;
    map <string, string> customTokens;
    bool ReadFromXML(void);
    bool Translate(string text, string &translation);
    void Debug(void);
};

#endif //__XMLGLOBALS_H