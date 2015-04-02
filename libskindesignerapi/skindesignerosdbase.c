#include "skindesignerosdbase.h"

/**********************************************************************
* cSkindesignerOsdObject
**********************************************************************/

skindesignerapi::cSkindesignerOsdObject::cSkindesignerOsdObject(void) {
    pluginName = "";
}

skindesignerapi::cSkindesignerOsdObject::~cSkindesignerOsdObject() {
}

bool skindesignerapi::cSkindesignerOsdObject::InitSkindesignerInterface(string pluginName) {
    this->pluginName = pluginName;
    return true;
}

skindesignerapi::cOsdView *skindesignerapi::cSkindesignerOsdObject::GetOsdView(int viewID, int subViewID) {
    ISkinDisplayPlugin *displayPlugin = SkindesignerAPI::GetDisplayPlugin(pluginName, viewID, subViewID);
    if (!displayPlugin)
        return NULL;
    cOsdView *view = new cOsdView(displayPlugin);
    return view;
}

/**********************************************************************
* cSkindesignerOsdItem
**********************************************************************/
skindesignerapi::cSkindesignerOsdItem::cSkindesignerOsdItem(eOSState State) : cOsdItem(State) {
    sdDisplayMenu = NULL;
}

skindesignerapi::cSkindesignerOsdItem::cSkindesignerOsdItem(const char *Text, eOSState State, bool Selectable) : cOsdItem(Text, State, Selectable) {
    sdDisplayMenu = NULL;
}  

skindesignerapi::cSkindesignerOsdItem::~cSkindesignerOsdItem() {

}

void skindesignerapi::cSkindesignerOsdItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable) {
    if (sdDisplayMenu) {
        if (!sdDisplayMenu->SetItemPlugin(&stringTokens, &intTokens, &loopTokens, Index, Current, Selectable)) {
            DisplayMenu->SetItem(Text(), Index, Current, Selectable);
        }
    } else {
        DisplayMenu->SetItem(Text(), Index, Current, Selectable);
    }
}

void skindesignerapi::cSkindesignerOsdItem::AddStringToken(string key, string value) {
    stringTokens.insert(pair<string,string>(key, value));
}

void skindesignerapi::cSkindesignerOsdItem::AddIntToken(string key, int value) {
    intTokens.insert(pair<string,int>(key, value));
}

void skindesignerapi::cSkindesignerOsdItem::AddLoopToken(string loopName, map<string, string> &tokens) {
    map<string, vector<map<string, string> > >::iterator hitLoop = loopTokens.find(loopName);
    if (hitLoop == loopTokens.end()) {
        vector<map<string, string> > tokenVector;
        tokenVector.push_back(tokens);
        loopTokens.insert(pair<string, vector<map<string, string> > >(loopName, tokenVector));
    } else {
        vector<map<string, string> > *tokenVector = &hitLoop->second;
        tokenVector->push_back(tokens);
    }
}


/**********************************************************************
* cSkindesignerOsdMenu
**********************************************************************/
skindesignerapi::cSkindesignerOsdMenu::cSkindesignerOsdMenu(const char *Title, int c0, int c1, int c2, int c3, int c4) : cOsdMenu(Title, c0, c1, c2, c3, c4) {
    init = true;
    displayText = false;
    sdDisplayMenu = NULL;
    pluginName = "";
    SetMenuCategory(mcPlugin);
    SetSkinDesignerDisplayMenu();
}

skindesignerapi::cSkindesignerOsdMenu::~cSkindesignerOsdMenu() {

}

void skindesignerapi::cSkindesignerOsdMenu::SetPluginMenu(int menu, eMenuType type) {
    if (type == mtList)
        displayText = false;
    else if (type == mtText)
        displayText = true;

    if (sdDisplayMenu) {
        sdDisplayMenu->SetPluginMenu(pluginName, menu, type, init);
    }
    init = false;
}

bool skindesignerapi::cSkindesignerOsdMenu::SetSkinDesignerDisplayMenu(void) {
    sdDisplayMenu = SkindesignerAPI::GetDisplayMenu();
    return (sdDisplayMenu != NULL);
}

void skindesignerapi::cSkindesignerOsdMenu::ClearTokens(void) {
    text = "";
    stringTokens.clear();
    intTokens.clear();
    loopTokens.clear();
}

void skindesignerapi::cSkindesignerOsdMenu::AddStringToken(string key, string value) {
    stringTokens.insert(pair<string,string>(key, value));
}

void skindesignerapi::cSkindesignerOsdMenu::AddIntToken(string key, int value) {
    intTokens.insert(pair<string,int>(key, value));
}

void skindesignerapi::cSkindesignerOsdMenu::AddLoopToken(string loopName, map<string, string> &tokens) {
    map<string, vector<map<string, string> > >::iterator hitLoop = loopTokens.find(loopName);
    if (hitLoop == loopTokens.end()) {
        vector<map<string, string> > tokenVector;
        tokenVector.push_back(tokens);
        loopTokens.insert(pair<string, vector<map<string, string> > >(loopName, tokenVector));
    } else {
        vector<map<string, string> > *tokenVector = &hitLoop->second;
        tokenVector->push_back(tokens);
    }
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyLeft(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(true, true);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyRight(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(false, true);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyUp(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(true, false);
}

void skindesignerapi::cSkindesignerOsdMenu::TextKeyDown(void) {
    if (!displayText)
        return;
    DisplayMenu()->Scroll(false, false);
}

void skindesignerapi::cSkindesignerOsdMenu::Display(void) {
    if (displayText) {
        if (sdDisplayMenu) {
            sdDisplayMenu->SetTitle(Title());
            if (sdDisplayMenu->SetPluginText(&stringTokens, &intTokens, &loopTokens)) {
                sdDisplayMenu->Flush();
            } else {
                DisplayMenu()->Clear();
                DisplayMenu()->SetTitle(Title());
                DisplayMenu()->SetText(text.c_str(), false);
                DisplayMenu()->Flush();
            }
        } else {
            DisplayMenu()->Clear();
            DisplayMenu()->SetTitle(Title());
            DisplayMenu()->SetText(text.c_str(), false);
            DisplayMenu()->Flush();
        }
        return;
    }
    if (sdDisplayMenu) {
        sdDisplayMenu->SetTitle(Title());
        for (cOsdItem *item = First(); item; item = Next(item)) {
            cSkindesignerOsdItem *sdItem = dynamic_cast<cSkindesignerOsdItem*>(item);
            if (sdItem) {
                sdItem->SetDisplayMenu(sdDisplayMenu);
            }
        }
    }
    cOsdMenu::Display();
}
