#include "skinsetup.h"
#include "../libtemplate/xmlparser.h"

// --- cSkinSetupParameter -----------------------------------------------------------

cSkinSetupParameter::cSkinSetupParameter(void) {
    type = sptUnknown;
    name = "";
    displayText = "";
    min = 0;
    max = 1000;
    value = 0; 
}

void cSkinSetupParameter::Debug(void) {
    string sType = "unknown";
    if (type == sptBool)
        sType = "bool";
    else if (type == sptInt)
        sType = "int";
    dsyslog("skindesigner: name \"%s\", type %s, displayText \"%s\", Value %d", name.c_str(), sType.c_str(), displayText.c_str(), value);
    if (type == sptInt)
        dsyslog("skindesigner: min %d, max %d", min, max);
}

// --- cSkinSetup -----------------------------------------------------------

cSkinSetup::cSkinSetup(string skin) {
	this->skin = skin;
}

cSkinSetup::~cSkinSetup() {
    for (map < string, cSkinSetupParameter* >::iterator p = parameters.begin(); p != parameters.end(); p++) {
        delete p->second;
    }
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
    
	cSkinSetupParameter *param = new cSkinSetupParameter();
	param->type = paramType;
	param->name = (const char*)name;
	param->displayText = (const char*)displayText;

	if (min && paramType == sptInt) {
		param->min = atoi((const char*)min);
	}
	if (max && paramType == sptInt) {
		param->max = atoi((const char*)max);
	}
	param->value = atoi((const char*)value);

	parameters.insert(pair< string, cSkinSetupParameter* >(param->name, param));
}

cSkinSetupParameter *cSkinSetup::GetParameter(void) {
    if (paramIt == parameters.end())
        return NULL;
    cSkinSetupParameter *param = paramIt->second;
    paramIt++;
    return param;
}

cSkinSetupParameter *cSkinSetup::GetParameter(string name) {
    map < string, cSkinSetupParameter* >::iterator hit = parameters.find(name);
    if (hit != parameters.end())
        return hit->second;
    return NULL;
}


void cSkinSetup::SetTranslation(string translationToken, map < string, string > transl) {
	translations.insert(pair<string, map < string, string > >(translationToken, transl));
}

void cSkinSetup::AddToGlobals(cGlobals *globals) {
    if (!globals)
    	return;
    for (map < string, cSkinSetupParameter* >::iterator p = parameters.begin(); p != parameters.end(); p++) {
    	cSkinSetupParameter *param = p->second;
		globals->AddInt(param->name, param->value);
    }
}

void cSkinSetup::TranslateSetup(void) {
    InitParameterIterator();
    cSkinSetupParameter *param = NULL;
    while (param = GetParameter()) {
        string transl = "";
        if (Translate(param->displayText, transl)) {
            param->displayText = transl;
        }
    }
}

bool cSkinSetup::Translate(string text, string &translation) {
    string transStart = "{tr(";
    string transEnd = ")}";
    size_t foundStart = text.find(transStart);
    size_t foundEnd = text.find(transEnd);
    bool translated = false;

    while (foundStart != string::npos && foundEnd != string::npos) {
        string token = text.substr(foundStart + 1, foundEnd - foundStart);
        string transToken = DoTranslate(token);
        if (transToken.size() > 0)
            translated = true;
        else
            return false;
        text.replace(foundStart, foundEnd - foundStart + 2, transToken);
        foundStart = text.find(transStart);
        foundEnd = text.find(transEnd);
    }
    if (translated)
        translation = text;
    return translated;
}

string cSkinSetup::DoTranslate(string token) {
    string translation = "";
    map <string, map< string, string > >::iterator hit = translations.find(token);
    if (hit == translations.end()) {
        esyslog("skindesigner: invalid translation token %s", token.c_str());
        return translation;
    }
    map< string, string > translats = hit->second;
    map< string, string >::iterator trans = translats.find(Setup.OSDLanguage);
    if (trans != translats.end()) {
        translation = trans->second;
    } else {
        map< string, string >::iterator transDefault = translats.find("en_EN");
        if (transDefault != translats.end()) {
            translation = transDefault->second;
        }
    }
    return translation;
}

void cSkinSetup::Debug(void) {
    dsyslog("skindesigner: Skin \"%s\" Setup Parameters", skin.c_str());
    for (map < string, cSkinSetupParameter* >::iterator p = parameters.begin(); p != parameters.end(); p++) {
    	(p->second)->Debug();
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
