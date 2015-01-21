#ifndef __SKINSETUP_H
#define __SKINSETUP_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <vdr/plugin.h>
#include <libxml/xmlstring.h>
#include "../libtemplate/globals.h"

using namespace std;

enum eSetupParameterType {
    sptInt,
    sptBool,
    sptUnknown
};

// --- cSkinSetupParameter -----------------------------------------------------------

class cSkinSetupParameter {
private:
public:
    cSkinSetupParameter(void);
    virtual ~cSkinSetupParameter(void) {};
    eSetupParameterType type;
    string name;
    string displayText;
    int min;
    int max;
    int value; 
    void Debug(void);
};

// --- cSkinSetup -----------------------------------------------------------

class cSkinSetup {
private:
	string skin;
	map < string, cSkinSetupParameter* > parameters;
    map < string, cSkinSetupParameter* >::iterator paramIt;
	map < string, map< string, string > > translations;
    string DoTranslate(string token);
    bool Translate(string text, string &translation);
public:
    cSkinSetup(string skin);
    virtual ~cSkinSetup(void);
    bool ReadFromXML(void);
    void SetParameter(xmlChar *type, xmlChar *name, xmlChar* displayText, xmlChar *min, xmlChar *max, xmlChar *value);
    void SetTranslation(string translationToken, map < string, string > transl);
    void AddToGlobals(cGlobals *globals);
    void TranslateSetup(void);
    void InitParameterIterator(void) { paramIt = parameters.begin(); };
    cSkinSetupParameter *GetParameter(void);
    cSkinSetupParameter *GetParameter(string name);
    string GetSkin(void) { return skin; };
    void Debug(void);
};

#endif //__SKINSETUP_H