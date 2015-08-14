#include "skinsetup.h"
#include "../config.h"
#include "../libtemplate/xmlparser.h"
#include "../libcore/helpers.h"

// --- cSkinSetupParameter -----------------------------------------------------------

cSkinSetupParameter::cSkinSetupParameter(void) {
    type = sptUnknown;
    name = "";
    displayText = "";
    helpText = "";
    min = 0;
    max = 1000;
    value = 0;
    options = NULL;
    optionsTranslated = NULL;
    numOptions = 0;
}

cSkinSetupParameter::~cSkinSetupParameter(void) {
    if (numOptions > 0 && options && optionsTranslated) {
        delete[] options;
        delete[] optionsTranslated;
    }
}

void cSkinSetupParameter::Debug(void) {
    string sType = "unknown";
    if (type == sptBool)
        sType = "bool";
    else if (type == sptInt)
        sType = "int";
    else if (type == sptString)
        sType = "string";
    dsyslog("skindesigner: name \"%s\", type %s, displayText \"%s\", Value: %d", name.c_str(), sType.c_str(), displayText.c_str(), value);
    if (type == sptInt)
        dsyslog("skindesigner: min %d, max %d", min, max);
    if (type == sptString && options) {
        for (int i=0; i < numOptions; i++) {
            dsyslog("skindesigner: option %d: %s", i+1, options[i]);
        }
    }
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

void cSkinSetupMenu::SetParameter(eSetupParameterType paramType, string name, string displayText, string helpText, string min, string max, string value, string options) {
    cSkinSetupParameter *param = new cSkinSetupParameter();
    param->type = paramType;
    param->name = name;
    param->displayText = displayText;
    param->helpText = helpText;

    if (min.size() && paramType == sptInt) {
        param->min = atoi(min.c_str());
    }
    if (max.size() && paramType == sptInt) {
        param->max = atoi(max.c_str());
    }

    param->value = atoi(value.c_str());

    if (paramType == sptString) {
        splitstring o(options.c_str());
        vector<string> opts = o.split(',', 1);
        int numOpts = opts.size();
        if (numOpts > 0) {
            param->numOptions = numOpts;
            param->options = new const char*[numOpts];
            int i=0;
            for (vector<string>::iterator it = opts.begin(); it != opts.end(); it++) {
                string option = trim(*it);
                char *s = new char[option.size()];
                strcpy(s, option.c_str());
                param->options[i++] = s;
            }
        }
    }

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

void cSkinSetup::SetSubMenu(string name, string displayText) {
    cSkinSetupMenu *subMenu = new cSkinSetupMenu();
    subMenu->SetName(name);
    subMenu->SetDisplayText(displayText);
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

void cSkinSetup::SetParameter(string type, string name, string displayText, string helpText, string min, string max, string value, string options) {
    if (!type.size() || !name.size() || !displayText.size() || !value.size()) {
        esyslog("skindesigner: invalid setup parameter for skin %s", skin.c_str());
        return;
    }
    eSetupParameterType paramType = sptUnknown;
    if (!type.compare("int")) {
        paramType = sptInt;
    } else if (!type.compare("bool")) {
        paramType = sptBool;
    } else if (!type.compare("string")) {
        paramType = sptString;
    }
    if (paramType == sptUnknown) {
        esyslog("skindesigner: invalid setup parameter for skin %s", skin.c_str());
        return;
    }
    currentMenu->SetParameter(paramType, name, displayText, helpText, min, max, value, options);
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
		if (param->type == sptString) {
            string value = param->options[param->value];
            globals->AddString(param->name, value);
            string intName = "index" + param->name;
            globals->AddInt(intName, param->value);
        } else {
            globals->AddInt(param->name, param->value);
        }
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
        string translHelp = "";
        if (Translate(param->helpText, translHelp)) {
            param->helpText = translHelp;
        }
        if (param->type == sptString && param->numOptions > 0) {
            param->optionsTranslated = new const char*[param->numOptions];
            for (int i = 0; i < param->numOptions; i++) {
                string option = param->options[i];
                string optionTransToken = "{tr(" + option + ")}";
                string optionTranslated = "";
                if (Translate(optionTransToken, optionTranslated)) {
                    char *s = new char[optionTranslated.size()];
                    strcpy(s, optionTranslated.c_str());
                    param->optionsTranslated[i] = s;
                } else {
                    char *s = new char[option.size()];
                    strcpy(s, option.c_str());
                    param->optionsTranslated[i] = s;
                }
            }
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
