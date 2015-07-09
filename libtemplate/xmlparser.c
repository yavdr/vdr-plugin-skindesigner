#include "xmlparser.h"
#include "../config.h"
#include "../libcore/helpers.h"

using namespace std;

cXmlParser::cXmlParser(void) {
    view = NULL;
    globals = NULL;
    skinSetup = NULL;
}

cXmlParser::~cXmlParser() {
}

/*********************************************************************
* PUBLIC Functions
*********************************************************************/
bool cXmlParser::ReadView(cTemplateView *view, string xmlFile) {
    if (!view)
        return false;
    this->view = view;
    string xmlPath = GetPath(xmlFile);
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName(view->GetViewName()) )
        return false;
    return true;
}

bool cXmlParser::ParseView(void) {
    if (!view)
        return false;

    vector<stringpair> rootAttribs = ParseAttributes();
    ValidateAttributes(NodeName(), rootAttribs);
    view->SetParameters(rootAttribs);

    if (!LevelDown())
        return false;

    do {

        if (view->ValidSubView(NodeName())) {
            ParseSubView();
        } else if (view->ValidViewElement(NodeName())) {
            ParseViewElement();
        } else if (view->ValidViewList(NodeName())) {
            ParseViewList();
        } else if (view->ValidViewGrid(NodeName())) {
            ParseGrid();
        } else if (CheckNodeName("tab")) {
            ParseViewTab(view);
        } else {
            return false;
        }

    } while (NextNode());

    return true;
    
}

bool cXmlParser::ReadPluginView(string plugName, int templateNumber, string templateName) {
    string xmlPath = GetPath(templateName);
    DeleteDocument();
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    return true;
}

bool cXmlParser::ParsePluginView(string plugName, int templateNumber) {
    cTemplateView *plugView = new cTemplateViewMenu();
    view->AddPluginView(plugName, templateNumber, plugView);

    vector<stringpair> attribs = ParseAttributes();
    ValidateAttributes(NodeName(), attribs);
    plugView->SetParameters(attribs);

    if (!LevelDown())
        return false;
    do {
        
        if (plugView->ValidViewElement(NodeName())) {
            ParseViewElement(plugView);
        } else if (plugView->ValidViewList(NodeName())) {
            ParseViewList(plugView);
        } else if (CheckNodeName("tab")) {
            ParseViewTab(plugView);      
        } else {
            return false;
        }

    } while (NextNode());

    return true;
}

bool cXmlParser::ReadGlobals(cGlobals *globals, string xmlFile) {
    this->globals = globals;
    string xmlPath = GetPath(xmlFile);
    DeleteDocument();
    if (! ReadXMLFile(xmlPath.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName("globals") )
        return false;
    return true;
}

bool cXmlParser::ParseGlobals(void) {
    if (!LevelDown())
        return false;
    do {
        if (CheckNodeName("colors")) {
            ParseGlobalColors();
        } else if (CheckNodeName("variables")) {
            ParseGlobalVariables();
        } else if (CheckNodeName("fonts")) {
            ParseGlobalFonts();
        } else if (CheckNodeName("translations")) {
            ParseTranslations();
        }
    } while (NextNode());
    return true;
}

bool cXmlParser::ReadSkinSetup(cSkinSetup *skinSetup, string xmlFile) {
    this->skinSetup = skinSetup;
    if (! ReadXMLFile(xmlFile.c_str()) )
        return false;
    if (! Validate() )
        return false;
    if (! SetDocument() )
        return false;
    if (! CheckNodeName("setup") )
        return false;
    return true;
}

bool cXmlParser::ParseSkinSetup(string skin) {
    if (!LevelDown())
        return false;
    do {
        if (CheckNodeName("menu")) {
            ParseSetupMenu();
        } else if (CheckNodeName("translations")) {
            ParseTranslations();
        }
    } while (NextNode());
    return true;
}
/*********************************************************************
* PRIVATE Functions
*********************************************************************/

bool cXmlParser::ParseSubView(void) {
    if (!view)
        return false;

    cTemplateView *subView = new cTemplateViewMenu();
    view->AddSubView(NodeName(), subView);

    vector<pair<string, string> > subViewAttribs = ParseAttributes();
    ValidateAttributes(NodeName(), subViewAttribs);
    subView->SetParameters(subViewAttribs);

    if (!LevelDown())
        return false;
    do {

        if (subView->ValidViewElement(NodeName())) {
            ParseViewElement(subView);
        } else if (subView->ValidViewList(NodeName())) {
            ParseViewList(subView);
        } else if (CheckNodeName("tab")) {
            ParseViewTab(subView);           
        } else {
            return false;
        }

    } while (NextNode());
    LevelUp();
    return true;

}
void cXmlParser::ParseViewElement(cTemplateView *subView) {
    if (!view)
        return;
    
    const char *viewElement = NodeName();
    vector<stringpair> attributes = ParseAttributes();
    ValidateAttributes("viewelement", attributes);

    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
            continue;
        }
        cTemplatePixmapNode *pix = NULL;
        if (CheckNodeName("area") || CheckNodeName("areascroll")) {
            pix = ParseArea();
        } else {
            pix = ParseAreaContainer();
        }
        if (subView)
            subView->AddPixmap(viewElement, pix, attributes);
        else
            view->AddPixmap(viewElement, pix, attributes);
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseViewList(cTemplateView *subView) {
    if (!view)
        return;
    
    vector<stringpair> attribs = ParseAttributes();
    ValidateAttributes(NodeName(), attribs);

    cTemplateViewList *viewList = new cTemplateViewList();
    viewList->SetGlobals(globals);
    viewList->SetParameters(attribs);

    if (!LevelDown())
        return;

    do {
        if (CheckNodeName("currentelement")) {

            cTemplateViewElement *currentElement = new cTemplateViewElement();
            vector<stringpair> attribsCur = ParseAttributes();
            ValidateAttributes(NodeName(), attribsCur);
            currentElement->SetGlobals(globals);
            currentElement->SetParameters(attribsCur);
            if (!LevelDown())
                return;
            do {
                if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
                    esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
                    continue;
                }
                cTemplatePixmapNode *pix = NULL;
                if (CheckNodeName("area") || CheckNodeName("areascroll")) {
                    pix = ParseArea();
                } else {
                    pix = ParseAreaContainer();
                }
                currentElement->AddPixmap(pix);
            } while (NextNode());
            LevelUp();
            viewList->AddCurrentElement(currentElement);

        } else if (CheckNodeName("listelement")) {
            
            cTemplateViewElement *listElement = new cTemplateViewElement();
            vector<stringpair> attribsList = ParseAttributes();
            ValidateAttributes(NodeName(), attribsList);
            listElement->SetGlobals(globals);
            listElement->SetParameters(attribsList);            
            if (!LevelDown())
                return;
            do {
                if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
                    esyslog("skindesigner: invalid tag \"%s\" in viewelement", NodeName());
                    continue;
                }
                cTemplatePixmapNode *pix = NULL;
                if (CheckNodeName("area") || CheckNodeName("areascroll")) {
                    pix = ParseArea();
                } else {
                    pix = ParseAreaContainer();
                }
                listElement->AddPixmap(pix);
            } while (NextNode());
            LevelUp();
            viewList->AddListElement(listElement);
        }

    } while (NextNode());
    LevelUp();

    if (subView)
        subView->AddViewList(NodeName(), viewList);
    else
        view->AddViewList(NodeName(), viewList);
}

void cXmlParser::ParseViewTab(cTemplateView *subView) {
    if (!view || !subView)
        return;

    vector<stringpair> attribs = ParseAttributes();
    ValidateAttributes(NodeName(), attribs);

    cTemplateViewTab *viewTab = new cTemplateViewTab();
    viewTab->SetGlobals(globals);
    viewTab->SetParameters(attribs);
    viewTab->SetScrolling();
    ParseFunctionCalls(viewTab);
    subView->AddViewTab(viewTab);
}

void cXmlParser::ParseGrid(void) {
    if (!view)
        return;

    vector<stringpair> attributes = ParseAttributes();
    ValidateAttributes(NodeName(), attributes);

    if (!LevelDown())
        return;
    do {

        if (!CheckNodeName("areacontainer") && !CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in grid", NodeName());
            continue;
        }
        cTemplatePixmapNode *pix = NULL;
        if (CheckNodeName("area") || CheckNodeName("areascroll")) {
            pix = ParseArea();
        } else {
            pix = ParseAreaContainer();
        }
        view->AddPixmapGrid(pix, attributes);
    } while (NextNode());
    LevelUp();
}

cTemplatePixmap *cXmlParser::ParseArea(void) {
    vector<stringpair> attribs = ParseAttributes();
    ValidateAttributes(NodeName(), attribs);

    cTemplatePixmap *pix = new cTemplatePixmap();
    if (CheckNodeName("areascroll")) {
        pix->SetScrolling();
    }
    pix->SetParameters(attribs);
    ParseFunctionCalls(pix);
    return pix;    
}

cTemplatePixmapContainer *cXmlParser::ParseAreaContainer(void) {
    vector<stringpair> attribs = ParseAttributes();
    ValidateAttributes(NodeName(), attribs);

    cTemplatePixmapContainer *pixContainer = new cTemplatePixmapContainer();
    pixContainer->SetParameters(attribs);

    if (!LevelDown())
        return pixContainer;
    do {
        if (!CheckNodeName("area") && !CheckNodeName("areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\" in areacontainer", NodeName());
            continue;
        }
        cTemplatePixmap *pix = ParseArea();
        pixContainer->AddPixmap(pix);
    } while (NextNode());
    LevelUp();
    return pixContainer;
}

void cXmlParser::ParseFunctionCalls(cTemplatePixmap *pix) {
    if (!view)
        return;
    if (!LevelDown())
        return;
    do {
        if (CheckNodeName("loop")) {
            vector<stringpair> attribs = ParseAttributes();
            ValidateAttributes(NodeName(), attribs);
            cTemplateLoopFunction *loopFunc = new cTemplateLoopFunction();
            loopFunc->SetParameters(attribs);
            ParseLoopFunctionCalls(loopFunc);
            pix->AddLoopFunction(loopFunc);
        } else if (view->ValidFunction(NodeName())) {
            vector<stringpair> attribs = ParseAttributes();
            ValidateAttributes(NodeName(), attribs);
            pix->AddFunction(NodeName(), attribs);
        }

    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseLoopFunctionCalls(cTemplateLoopFunction *loopFunc) {
    if (!view)
        return;
    if (!LevelDown())
        return;
    do {
        if (view->ValidFunction(NodeName())) {
            vector<stringpair> attribs = ParseAttributes();
            ValidateAttributes(NodeName(), attribs);
            loopFunc->AddFunction(NodeName(), attribs);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseGlobalColors(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("color")) {
            continue;
        }
        string attributeName = "name";
        string colorName  = "";
        string colorValue = "";
        bool ok = GetAttribute(attributeName, colorName);
        if (ok) {
            ok = GetNodeValue(colorValue);
            if (ok)
                InsertColor(colorName, colorValue);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::InsertColor(string name, string value) {
    if (value.size() != 8)
        return;
    std::stringstream str;
    str << value;
    tColor colVal;
    str >> std::hex >> colVal;
    globals->AddColor(name, colVal);
}

void cXmlParser::ParseGlobalVariables(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("var")) {
            continue;
        }
        string attributeName = "name";
        string attributeType = "type";
        string varName  = "";
        string varType  = "";
        string varValue = "";

        bool ok1 = GetAttribute(attributeName, varName);
        bool ok2 = GetAttribute(attributeType, varType);

        if (ok1 && ok2) {
            bool ok = GetNodeValue(varValue);
            if (ok)
                InsertVariable(varName, varType, varValue);
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::InsertVariable(string name, string type, string value) {
    if (!type.compare("int")) {
        int val = atoi(value.c_str());
        globals->AddInt(name, val);
    } else if (!type.compare("double")) {
        globals->AddDouble(name, value);
    } else if (!type.compare("string")) {
        globals->AddString(name, value);
    }
}

void cXmlParser::ParseGlobalFonts(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("font")) {
            continue;
        }
        string attributeName = "name";
        string fontName = "";
        string fontValue = "";

        bool ok = GetAttribute(attributeName, fontName);
        if (ok) {
            ok = GetNodeValue(fontValue);
            if (ok) {
                globals->AddFont(fontName, fontValue);
            }
        }
    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseTranslations(void) {
    if (!LevelDown())
        return;
    do {
        if (!CheckNodeName("token")) {
            continue;
        }
        string attributeName = "name";
        string tokenName = "";

        if (!GetAttribute(attributeName, tokenName))
            continue;

        if (!LevelDown())
            continue;

        stringmap tokenTranslations;
        do {
            if (!CheckNodeName("trans")) {
                continue;
            }
            string attributeName = "lang";
            string language = "";
            if (!GetAttribute(attributeName, language))
                continue;
            string translation = "";
            if (!GetNodeValue(translation))
                continue;
            tokenTranslations.insert(stringpair(language, translation));
        } while (NextNode());
        LevelUp();

        if (globals) {
            globals->AddTranslation(tokenName, tokenTranslations);
        } else if (skinSetup) {
            skinSetup->SetTranslation(tokenName, tokenTranslations);
        }

    } while (NextNode());
    LevelUp();
}

void cXmlParser::ParseSetupMenu(void) {
    if (!skinSetup)
        return;
    if (!LevelDown())
        return;
    do {
        if (CheckNodeName("parameter")) {
            ParseSetupParameter();
        } else if (CheckNodeName("submenu")) {
            string attributeName = "name";
            string subMenuName = "";
            string attributeDisplayText = "displaytext";
            string subDisplayText = "";
            GetAttribute(attributeName, subMenuName);
            GetAttribute(attributeDisplayText, subDisplayText);
            skinSetup->SetSubMenu(subMenuName, subDisplayText);
            ParseSetupMenu();
        }
    } while (NextNode());
    skinSetup->SubMenuDone();
    LevelUp();
}

void cXmlParser::ParseSetupParameter(void) {
    if (!skinSetup)
        return;
    string attributeType = "type";
    string paramType = "";
    string attributeName = "name";
    string paramName = "";
    string attributeDisplayText = "displaytext";
    string paramDisplayText = "";
    string attributeMin = "min";
    string paramMin = "";
    string attributeMax = "max";
    string paramMax = "";
    string paramValue = "";

    GetAttribute(attributeType, paramType);
    GetAttribute(attributeName, paramName);
    GetAttribute(attributeDisplayText, paramDisplayText);
    GetAttribute(attributeMin, paramMin);
    GetAttribute(attributeMax, paramMax);
    GetNodeValue(paramValue);

    skinSetup->SetParameter(paramType, paramName, paramDisplayText, paramMin, paramMax, paramValue);
}

void cXmlParser::ValidateAttributes(const char *nodeName, vector<stringpair> &attributes) {
    bool repeat = true;
    while (repeat) {
        repeat  = false;
        for (vector<stringpair>::iterator it = attributes.begin(); it != attributes.end(); it++) {
            string attributeName = (*it).first;
            if (!view->ValidAttribute(nodeName, attributeName.c_str())) {
                attributes.erase(it);
                repeat = true;
                break;
            }
        }
    }
}

string cXmlParser::GetPath(string xmlFile) {
    string activeSkin = Setup.OSDSkin;
    string activeTheme = Setup.OSDTheme;
    string path = "";
    if (!xmlFile.compare("globals.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("theme.xml")) {
        path = *cString::sprintf("%s%s/themes/%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), activeTheme.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("setup.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    } else {
        path = *cString::sprintf("%s%s/xmlfiles/%s", *config.GetSkinPath(activeSkin), activeSkin.c_str(), xmlFile.c_str());
    }
    return path;
}

