#include "skinsetup.h"
#include "../libtemplate/xmlparser.h"

cSkinSetup::cSkinSetup(string skin) {
	this->skin = skin;
}

void cSkinSetup::ReadFromXML(void) {
    esyslog("skindesigner: reading setup for skin %s", skin.c_str());
    string xmlFile = "setup.xml";
    cXmlParser parser;
    if (!parser.ReadSkinSetup(this, skin, xmlFile)) {
        esyslog("skindesigner: no setup file for skin %s found", skin.c_str());
        return;
    }
    parser.ParseSkinSetup(skin);
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

	parameters.push_back(param);
}


void cSkinSetup::Debug(void) {
    dsyslog("skindesigner: Skin \"%s\" Setup Parameters", skin.c_str());
    for (vector<cSkinSetupParameter>::iterator p = parameters.begin(); p != parameters.end(); p++)
    	p->Debug();
}
