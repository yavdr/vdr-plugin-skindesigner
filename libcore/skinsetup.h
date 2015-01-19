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
#include "skinsetupparameter.h"

using namespace std;

// --- cSkinSetup -----------------------------------------------------------

class cSkinSetup {
private:
	string skin;
	map <string, cSkinSetupParameter> parameters;
	map <string, map< string, string > > translations;
public:
    cSkinSetup(string skin);
    virtual ~cSkinSetup(void) {};
    bool ReadFromXML(void);
    void SetParameter(xmlChar *type, xmlChar *name, xmlChar* displayText, xmlChar *min, xmlChar *max, xmlChar *value);
    void SetTranslation(string translationToken, map < string, string > transl);
    void AddToGlobals(cGlobals *globals);
    void Debug(void);
};

#endif //__SKINSETUP_H