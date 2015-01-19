#ifndef __SKINSETUP_H
#define __SKINSETUP_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <vdr/plugin.h>
#include <libxml/xmlstring.h>
#include "skinsetupparameter.h"

using namespace std;

// --- cSkinSetup -----------------------------------------------------------

class cSkinSetup {
private:
	string skin;
	vector<cSkinSetupParameter> parameters;
public:
    cSkinSetup(string skin);
    virtual ~cSkinSetup(void) {};
    void ReadFromXML(void);
    void SetParameter(xmlChar *type, xmlChar *name, xmlChar* displayText, xmlChar *min, xmlChar *max, xmlChar *value);
    void Debug(void);
};

#endif //__SKINSETUP_H