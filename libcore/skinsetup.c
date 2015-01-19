#include "skinsetup.h"
#include "../libtemplate/xmlparser.h"

cSkinSetup::cSkinSetup(string skin) {
	this->skin = skin;
}

bool cSkinSetup::ReadFromXML(void) {
    string xmlFile = "setup.xml";
    cXmlParser parser;
    if (!parser.ReadSkinSetup(this, skin, xmlFile)) {
        return false;
    }
    parser.ParseSkinSetup(skin);
    return true;
}

void cSkinSetup::SetParameter(xmlChar *type, xmlChar *name, xmlChar* displayText, xmlChar *min, xmlChar *max, xmlChar *value) {
	if (!type || !name || !displayText || !value) {
		esyslog("skindesigner: invalid setup parameter for skin %s", skin.c_str());
		return;
	}
	eSetupParameterType paramType = sptUnknown;
	if (!xmlStrcmp(type, (const xmlChar *) "int")) {
		paramType = sptInt;
	} else if (!xmlStrcmp(type, (const xmlChar *) "bool")) {
		paramType = sptBool;
	}
	if (paramType == sptUnknown) {
		esyslog("skindesigner: invalid setup parameter for skin %s", skin.c_str());
		return;
	}
	cSkinSetupParameter param;
	param.type = paramType;
	param.name = (const char*)name;
	param.displayText = (const char*)displayText;

	if (min && paramType == sptInt) {
		param.min = atoi((const char*)min);
	}
	if (max && paramType == sptInt) {
		param.max = atoi((const char*)max);
	}
	param.value = atoi((const char*)value);

	parameters.insert(pair<string, cSkinSetupParameter>(param.name, param));
}

void cSkinSetup::SetTranslation(string translationToken, map < string, string > transl) {
	translations.insert(pair<string, map < string, string > >(translationToken, transl));
}

void cSkinSetup::AddToGlobals(cGlobals *globals) {
    if (!globals)
    	return;
    for (map<string, cSkinSetupParameter>::iterator p = parameters.begin(); p != parameters.end(); p++) {
    	string paramName = p->first;
    	cSkinSetupParameter param = p->second;
		globals->intVars.erase(paramName);
		globals->intVars.insert(pair<string,int>(paramName, param.value));
    }
}

void cSkinSetup::Debug(void) {
    dsyslog("skindesigner: Skin \"%s\" Setup Parameters", skin.c_str());
    for (map<string, cSkinSetupParameter>::iterator p = parameters.begin(); p != parameters.end(); p++) {
    	(p->second).Debug();
    }
    dsyslog("skindesigner: Skin \"%s\" Setup Parameter Translations", skin.c_str());
	for (map<string, map<string,string> >::iterator trans = translations.begin(); trans != translations.end(); trans++) {
		dsyslog("skindesigner: translation token %s", (trans->first).c_str());
		map<string,string> transValues = trans->second;
		for (map<string,string>::iterator trans2 = transValues.begin(); trans2 != transValues.end(); trans2++) {
			dsyslog("skindesigner: translation language %s value \"%s\"", (trans2->first).c_str(), (trans2->second).c_str());			
		}
	}    
}
