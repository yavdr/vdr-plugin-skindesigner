#include "skinsetup.h"
#include "../config.h"
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

// --- cSkinSetupMenu -----------------------------------------------------------
cSkinSetupMenu::cSkinSetupMenu(void) {
    name = "";
    displayText = "";
    parent = NULL;
}

cSkinSetupMenu::~cSkinSetupMenu(void) {
    for (vector < cSkinSetupParameter* >::iterator p = parameters.begin(); p != parameters.end(); p++) {
        delete (*p);
    }
    for (vector < cSkinSetupMenu* >::iterator s = subMenus.begin(); s != subMenus.end(); s++) {
        delete (*s);
    }
}

cSkinSetupParameter *cSkinSetupMenu::GetNextParameter(bool deep) {
    cSkinSetupParameter *param = NULL;
    if (paramIt != parameters.end()) {
        param = *paramIt;
        paramIt++;
        return param;
    }
    if (!deep)
        return NULL;

    if (subMenuIt != subMenus.end()) {
        param = (*subMenuIt)->GetNextParameter();
        if (!param) {
            subMenuIt++;
            if (subMenuIt != subMenus.end()) {
                (*subMenuIt)->InitIterators();
                param = (*subMenuIt)->GetNextParameter();
            }
        }
    }
    return param;
}

cSkinSetupParameter *cSkinSetupMenu::GetParameter(string name) {
    for (vector < cSkinSetupParameter* >::iterator it = parameters.begin(); it != parameters.end(); it++) {
        if (!name.compare((*it)->name))
            return *it;
    }

    cSkinSetupParameter *paramHit = NULL;
    for (vector < cSkinSetupMenu* >::iterator subMenu = subMenus.begin(); subMenu != subMenus.end(); subMenu++) {
        paramHit = (*subMenu)->GetParameter(name);
        if (paramHit)
            return paramHit;
    }
    return NULL;
}

void cSkinSetupMenu::InitIterators(void) {
    paramIt = parameters.begin();
    subMenuIt = subMenus.begin();
    while (subMenuIt != subMenus.end()) {
        (*subMenuIt)->InitIterators();
        subMenuIt++;
    }
    subMenuIt = subMenus.begin();
}

void cSkinSetupMenu::SetParameter(eSetupParameterType paramType, xmlChar *name, xmlChar* displayText, xmlChar *min, xmlChar *max, xmlChar *value) {
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

    parameters.push_back(param);
}

cSkinSetupMenu *cSkinSetupMenu::GetMenu(string &name) {
    for (vector<cSkinSetupMenu*>::iterator m = subMenus.begin(); m != subMenus.end(); m++) {
        cSkinSetupMenu *menu = (*m);
        if (!name.compare(menu->GetName()))
            return menu;
        menu = menu->GetMenu(name);
        if (menu)
            return menu;  
    }
    return NULL;
}

cSkinSetupMenu *cSkinSetupMenu::GetNextSubMenu(bool deep) {
    cSkinSetupMenu *menu = NULL;
    if (subMenuIt != subMenus.end()) {
        if (deep) {
            menu = (*subMenuIt)->GetNextSubMenu(deep);
            if (menu)
                return menu;
        }
        menu = *subMenuIt;
        subMenuIt++;
        return menu;
    }
    return NULL;
}


void cSkinSetupMenu::Debug(bool deep) {
    dsyslog("skindesigner: Menu %s Setup Parameters", name.c_str());
    for (vector < cSkinSetupParameter* >::iterator p = parameters.begin(); p != parameters.end(); p++) {
        (*p)->Debug();
    }
    if (subMenus.empty())
        return;
    for (vector < cSkinSetupMenu* >::iterator s = subMenus.begin(); s != subMenus.end(); s++) {
        dsyslog("skindesigner: SubMenu %s, Parent %s", ((*s)->GetName()).c_str(), ((*s)->GetParent()->GetName()).c_str());
        if (deep)
            (*s)->Debug();
    }
}
// --- cSkinSetup -----------------------------------------------------------

cSkinSetup::cSkinSetup(string skin) {
	this->skin = skin;
    rootMenu = new cSkinSetupMenu();
    rootMenu->SetName("root");
    currentMenu = rootMenu;
}

cSkinSetup::~cSkinSetup() {
    delete rootMenu;
}

bool cSkinSetup::ReadFromXML(void) {
    string xmlPath = *cString::sprintf("%s%s/setup.xml", *config.GetSkinPath(skin), skin.c_str());
    cXmlParser parser;
    if (!parser.ReadSkinSetup(this, xmlPath)) {
        return false;
    }
    parser.ParseSkinSetup(skin);
    return true;
}

void cSkinSetup::SetSubMenu(xmlChar *name, xmlChar *displayText) {
    cSkinSetupMenu *subMenu = new cSkinSetupMenu();
    subMenu->SetName((const char*)name);
    subMenu->SetDisplayText((const char*)displayText);
    subMenu->SetParent(currentMenu);
    currentMenu->AddSubMenu(subMenu);
    currentMenu = subMenu;
}

void cSkinSetup::SubMenuDone(void) {
    cSkinSetupMenu *parent = currentMenu->GetParent();
    if (parent) {
        currentMenu = parent;
    }
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
    currentMenu->SetParameter(paramType, name, displayText, min, max, value);
}

cSkinSetupParameter *cSkinSetup::GetNextParameter(void) {
    return rootMenu->GetNextParameter();
}

cSkinSetupParameter *cSkinSetup::GetParameter(string name) {
    return rootMenu->GetParameter(name);
}

void cSkinSetup::SetTranslation(string translationToken, map < string, string > transl) {
	translations.insert(pair<string, map < string, string > >(translationToken, transl));
}

void cSkinSetup::AddToGlobals(cGlobals *globals) {
    if (!globals)
    	return;
    rootMenu->InitIterators();
    cSkinSetupParameter *param = NULL;
    while (param = rootMenu->GetNextParameter()) {
		globals->AddInt(param->name, param->value);
    }
}

void cSkinSetup::TranslateSetup(void) {
    rootMenu->InitIterators();
    cSkinSetupParameter *param = NULL;
    while (param = rootMenu->GetNextParameter()) {
        string transl = "";
        if (Translate(param->displayText, transl)) {
            param->displayText = transl;
        }
    }

    rootMenu->InitIterators();
    cSkinSetupMenu *subMenu = NULL;
    while (subMenu = rootMenu->GetNextSubMenu()) {
        string transl = "";
        if (Translate(subMenu->GetDisplayText(), transl)) {
            subMenu->SetDisplayText(transl);
        }
    }
}

cSkinSetupMenu *cSkinSetup::GetMenu(string &name) {
    if (name.size() == 0)
        return rootMenu;
    return rootMenu->GetMenu(name);
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
    rootMenu->Debug();
    return;
    dsyslog("skindesigner: Skin \"%s\" Setup Parameter Translations", skin.c_str());
	for (map<string, map<string,string> >::iterator trans = translations.begin(); trans != translations.end(); trans++) {
		dsyslog("skindesigner: translation token %s", (trans->first).c_str());
		map<string,string> transValues = trans->second;
		for (map<string,string>::iterator trans2 = transValues.begin(); trans2 != transValues.end(); trans2++) {
			dsyslog("skindesigner: translation language %s value \"%s\"", (trans2->first).c_str(), (trans2->second).c_str());			
		}
	}

}
