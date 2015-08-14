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
    sptString,
    sptUnknown
};

// --- cSkinSetupParameter -----------------------------------------------------------

class cSkinSetupParameter {
private:
public:
    cSkinSetupParameter(void);
    virtual ~cSkinSetupParameter(void);
    eSetupParameterType type;
    string name;
    string displayText;
    string helpText;
    int min;
    int max;
    int value;
    const char* *options;
    const char* *optionsTranslated;
    int numOptions;
    void Debug(void);
};

// --- cSkinSetupMenu -----------------------------------------------------------

class cSkinSetupMenu {
private:
    string name;
    string displayText;
    cSkinSetupMenu *parent;
    vector < cSkinSetupMenu* > subMenus;
    vector < cSkinSetupMenu* >::iterator subMenuIt;
    vector < cSkinSetupParameter* > parameters;
    vector < cSkinSetupParameter* >::iterator paramIt;
public:
    cSkinSetupMenu(void);
    virtual ~cSkinSetupMenu(void);
    void SetName(string name) { this->name = name; };
    void SetDisplayText(string displayText) { this->displayText = displayText; };
    string GetName(void) { return name; };
    string GetDisplayText(void) { return displayText; };
    void SetParent(cSkinSetupMenu *p) { parent = p; };
    cSkinSetupMenu *GetParent(void) { return parent; };
    void AddSubMenu(cSkinSetupMenu *sub) { subMenus.push_back(sub); };
    void SetParameter(eSetupParameterType paramType, string name, string displayText, string helpText, string min, string max, string value, string options);
    void InitIterators(void);
    void InitParameterIterator(void) { paramIt = parameters.begin(); };
    cSkinSetupParameter *GetNextParameter(bool deep = true);
    cSkinSetupParameter *GetParameter(string name);
    void InitSubmenuIterator(void) { subMenuIt = subMenus.begin(); };
    cSkinSetupMenu *GetNextSubMenu(bool deep = true);
    cSkinSetupMenu *GetMenu(string &name);
    void Debug(bool deep = true);
};

// --- cSkinSetup -----------------------------------------------------------

class cSkinSetup {
private:
    string skin;
    cSkinSetupMenu *rootMenu;
    cSkinSetupMenu *currentMenu;
    map < string, map< string, string > > translations;
    string DoTranslate(string token);
    bool Translate(string text, string &translation);
public:
    cSkinSetup(string skin);
    virtual ~cSkinSetup(void);
    bool ReadFromXML(void);
    void SetSubMenu(string name, string displayText);
    void SubMenuDone(void);
    void SetParameter(string type, string name, string displayText, string helpText, string min, string max, string value, string options);
    void InitParameterIterator(void) { rootMenu->InitIterators(); };
    cSkinSetupParameter *GetNextParameter(void);
    cSkinSetupParameter *GetParameter(string name);
    void SetTranslation(string translationToken, map < string, string > transl);
    void AddToGlobals(cGlobals *globals);
    void TranslateSetup(void);
    string GetSkin(void) { return skin; };
    cSkinSetupMenu *GetMenu(string &name);
    void Debug(void);
};

#endif //__SKINSETUP_H