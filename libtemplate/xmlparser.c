#include "xmlparser.h"
#include "../config.h"
#include "../libcore/helpers.h"

using namespace std;

void SkinDesignerXMLErrorHandler (void * userData, xmlErrorPtr error) {
    esyslog("skindesigner: Error in XML: %s", error->message);
}

cXmlParser::cXmlParser(void) {
    view = NULL;
    doc = NULL;
    root = NULL;
    ctxt = NULL;
    globals = NULL;
    skinSetup = NULL;

    initGenericErrorDefaultFunc(NULL);
    xmlSetStructuredErrorFunc(NULL, SkinDesignerXMLErrorHandler);
    ctxt = xmlNewParserCtxt();
}

cXmlParser::~cXmlParser() {
    DeleteDocument();
    xmlFreeParserCtxt(ctxt);
}

/*********************************************************************
* PUBLIC Functions
*********************************************************************/
bool cXmlParser::ReadView(cTemplateView *view, string xmlFile) {
    this->view = view;

    string xmlPath = GetPath(xmlFile);
    
    if (ctxt == NULL) {
        esyslog("skindesigner: Failed to allocate parser context");
        return false;
    }
    
    doc = xmlCtxtReadFile(ctxt, xmlPath.c_str(), NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);
    
    if (doc == NULL) {
        esyslog("skindesigner: ERROR: TemplateView %s not parsed successfully.", xmlPath.c_str());
        return false;
    }
    if (ctxt->valid == 0) {
        esyslog("skindesigner: Failed to validate %s", xmlPath.c_str());
        return false;
    }

    root = xmlDocGetRootElement(doc);

    if (root == NULL) {
        esyslog("skindesigner: ERROR: TemplateView %s is empty", xmlPath.c_str());
        return false;
    }

    if (xmlStrcmp(root->name, (const xmlChar *) view->GetViewName())) {
        return false;
    }
    return true;
}

bool cXmlParser::ReadPluginView(string plugName, int templateNumber, string templateName) {

    string xmlPath = GetPath(templateName);

    if (!FileExists(xmlPath) || ctxt == NULL) {
        return false;
    }
    DeleteDocument();
    doc = xmlCtxtReadFile(ctxt, xmlPath.c_str(), NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);
    
    if (doc == NULL) {
        return false;
    }
    if (ctxt->valid == 0) {
        esyslog("skindesigner: Failed to validate %s", xmlPath.c_str());
        return false;
    }

    root = xmlDocGetRootElement(doc);

    if (root == NULL) {
        return false;
    }

    return true;
}

bool cXmlParser::ReadGlobals(cGlobals *globals, string xmlFile, bool mandatory) {
    this->globals = globals;
    
    string xmlPath = GetPath(xmlFile);

    if (ctxt == NULL) {
        esyslog("skindesigner: Failed to allocate parser context");
        return false;
    }
    
    doc = xmlCtxtReadFile(ctxt, xmlPath.c_str(), NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);

    if (doc == NULL) {
        if (mandatory) {
            esyslog("skindesigner: ERROR: Globals %s not parsed successfully.", xmlPath.c_str());
        } else {
            dsyslog("skindesigner: Globals %s not parsed successfully.", xmlPath.c_str());
        }
        return false;
    }

    root = xmlDocGetRootElement(doc);

    if (ctxt->valid == 0) {
        if (mandatory) {
            esyslog("skindesigner: ERROR: Failed to validate %s", xmlPath.c_str());
        } else {
            dsyslog("skindesigner: Failed to validate %s", xmlPath.c_str());
        }
        return false;
    }

    if (root == NULL) {
        if (mandatory) {
            esyslog("skindesigner: ERROR: Globals %s is empty", xmlPath.c_str());
        } 
        return false;
    }

    if (xmlStrcmp(root->name, (const xmlChar *) "globals")) {
        return false;
    }
    return true;
}

bool cXmlParser::ReadSkinSetup(cSkinSetup *skinSetup, string xmlFile) {
    this->skinSetup = skinSetup;

    if (!FileExists(xmlFile))
        return false;
    if (ctxt == NULL) {
        esyslog("skindesigner: Failed to allocate parser context");
        return false;
    }
    
    doc = xmlCtxtReadFile(ctxt, xmlFile.c_str(), NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);

    if (doc == NULL ) {
        esyslog("skindesigner: ERROR: skin setup %s not parsed successfully.", xmlFile.c_str());
        return false;
    }

    root = xmlDocGetRootElement(doc);

    if (ctxt->valid == 0) {
        esyslog("skindesigner: Failed to validate %s", xmlFile.c_str());
        return false;
    }
    if (root == NULL) {
        return false;
    }
    if (xmlStrcmp(root->name, (const xmlChar *) "setup")) {
        return false;
    }
    
    return true;
}


bool cXmlParser::ParseView(void) {
    vector<pair<string, string> > rootAttribs;
    ParseAttributes(root->properties, root, rootAttribs);

    if (!view)
        return false;

    view->SetParameters(rootAttribs);

    xmlNodePtr node = root->xmlChildrenNode;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (view->ValidSubView((const char*)node->name)) {
            ParseSubView(node);
        } else if (view->ValidViewElement((const char*)node->name)) {
            xmlAttrPtr attr = node->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, node, attribs, true);
            ParseViewElement(node->name, node->xmlChildrenNode, attribs);
        } else if (view->ValidViewList((const char*)node->name)) {
            ParseViewList(node);
        } else if (view->ValidViewGrid((const char*)node->name)) {
            xmlAttrPtr attr = node->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, node, attribs);
            ParseGrid(node->xmlChildrenNode, attribs);
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "tab")) {
            ParseViewTab(node, view);
        } else {
            return false;
        }

        node = node->next;
    }

    return true;
    
}

bool cXmlParser::ParsePluginView(string plugName, int templateNumber) {

    cTemplateView *plugView = new cTemplateViewMenu();
    view->AddPluginView(plugName, templateNumber, plugView);

    vector<pair<string, string> > attribs;
    ParseAttributes(root->properties, root, attribs);

    plugView->SetParameters(attribs);

    xmlNodePtr childNode = root->xmlChildrenNode;

    while (childNode != NULL) {

        if (childNode->type != XML_ELEMENT_NODE) {
            childNode = childNode->next;
            continue;
        }

        if (plugView->ValidViewElement((const char*)childNode->name)) {
            vector<pair<string, string> > attribs;
            ParseViewElement(childNode->name, childNode->xmlChildrenNode, attribs, plugView);
        } else if (plugView->ValidViewList((const char*)childNode->name)) {
            ParseViewList(childNode, plugView);
        } else if (!xmlStrcmp(childNode->name, (const xmlChar *) "tab")) {
            ParseViewTab(childNode, plugView);           
        } else {
            return false;
        }

        childNode = childNode->next;
    }

    return true;
}

bool cXmlParser::ParseGlobals(void) {
    xmlNodePtr node = root->xmlChildrenNode;
    while (node != NULL) {
        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (!xmlStrcmp(node->name, (const xmlChar *) "colors")) {
            ParseGlobalColors(node->xmlChildrenNode);
            node = node->next;
            continue;
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "variables")) {
            ParseGlobalVariables(node->xmlChildrenNode);
            node = node->next;
            continue;
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "fonts")) {
            ParseGlobalFonts(node->xmlChildrenNode);
            node = node->next;
            continue;
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "translations")) {
            ParseTranslations(node->xmlChildrenNode);
            node = node->next;
            continue;
        }
        node = node->next;
    }

    return true;

}

bool cXmlParser::ParseSkinSetup(string skin) {
    xmlNodePtr node = root->xmlChildrenNode;

    while (node != NULL) {
        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (!xmlStrcmp(node->name, (const xmlChar *) "menu")) {
            ParseSetupMenu(node->xmlChildrenNode);
            node = node->next;
            continue;
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "translations")) {
            ParseTranslations(node->xmlChildrenNode);
            node = node->next;
            continue;
        }
        node = node->next;
    }

    return true;

}

void cXmlParser::DeleteDocument(void) {
    if (doc) {
        xmlFreeDoc(doc);
        doc = NULL;
    }
}

/*********************************************************************
* PRIVATE Functions
*********************************************************************/

string cXmlParser::GetPath(string xmlFile) {
    string activeSkin = Setup.OSDSkin;
    string activeTheme = Setup.OSDTheme;
    string path = "";
    if (!xmlFile.compare("globals.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.skinPath, activeSkin.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("theme.xml")) {
        path = *cString::sprintf("%s%s/themes/%s/%s", *config.skinPath, activeSkin.c_str(), activeTheme.c_str(), xmlFile.c_str());
    } else if (!xmlFile.compare("setup.xml")) {
        path = *cString::sprintf("%s%s/%s", *config.skinPath, activeSkin.c_str(), xmlFile.c_str());
    } else {
        path = *cString::sprintf("%s%s/xmlfiles/%s", *config.skinPath, activeSkin.c_str(), xmlFile.c_str());
    }
    return path;
}

void cXmlParser::ParseSetupMenu(xmlNodePtr node) {
    if (!node)
        return;
    if (!skinSetup)
        return;

    while (node != NULL) {
        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }

        if (!xmlStrcmp(node->name, (const xmlChar *) "parameter")) {
            ParseSetupParameter(node);
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "submenu")) {
            xmlAttrPtr attr = node->properties;
            xmlChar *subMenuName = NULL;
            xmlChar *subDisplayText = NULL;
            while (NULL != attr) {
                if (!xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                    subMenuName = xmlGetProp(node, attr->name);
                } else if (!xmlStrcmp(attr->name, (const xmlChar *) "displaytext")) {
                    subDisplayText = xmlGetProp(node, attr->name);
                }
                attr = attr->next;
            }
            skinSetup->SetSubMenu(subMenuName, subDisplayText);
            ParseSetupMenu(node->xmlChildrenNode);
            if (subMenuName)
                xmlFree(subMenuName);
            if (subDisplayText)
                xmlFree(subDisplayText);
        }
        node = node->next;
    }
    skinSetup->SubMenuDone();
}

void cXmlParser::ParseSetupParameter(xmlNodePtr node) {
    if (!node)
        return;
    if (!skinSetup)
        return;

    xmlAttrPtr attr = node->properties;
    if (attr == NULL) {
        return;
    }
    xmlChar *paramType = NULL;
    xmlChar *paramName = NULL;
    xmlChar *paramDisplayText = NULL;
    xmlChar *paramMin = NULL;
    xmlChar *paramMax = NULL;
    xmlChar *paramValue = NULL;
    while (NULL != attr) {
        if (!xmlStrcmp(attr->name, (const xmlChar *) "type")) {
            paramType = xmlGetProp(node, attr->name);
        } else if (!xmlStrcmp(attr->name, (const xmlChar *) "name")) {
            paramName = xmlGetProp(node, attr->name);
        } else if (!xmlStrcmp(attr->name, (const xmlChar *) "displaytext")) {
            paramDisplayText = xmlGetProp(node, attr->name);
        } else if (!xmlStrcmp(attr->name, (const xmlChar *) "min")) {
            paramMin = xmlGetProp(node, attr->name);
        } else if (!xmlStrcmp(attr->name, (const xmlChar *) "max")) {
            paramMax = xmlGetProp(node, attr->name);
        }
        attr = attr->next;
    }
    paramValue = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
    skinSetup->SetParameter(paramType, paramName, paramDisplayText, paramMin, paramMax, paramValue);
    if (paramType)
        xmlFree(paramType);
    if (paramName)
        xmlFree(paramName);
    if (paramDisplayText)
        xmlFree(paramDisplayText);
    if (paramMin)
        xmlFree(paramMin);
    if (paramMax)
        xmlFree(paramMax);
    if (paramValue)
        xmlFree(paramValue);
}

void cXmlParser::ParseGlobalColors(xmlNodePtr node) {
    if (!node)
        return;
    
    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (xmlStrcmp(node->name, (const xmlChar *) "color")) {
            node = node->next;
            continue;
        }
        xmlAttrPtr attr = node->properties;
        if (attr == NULL) {
            node = node->next;
            continue;
        }
        xmlChar *colName = NULL;
        xmlChar *colValue = NULL;
        bool ok = false;
        while (NULL != attr) {
            if (xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                attr = attr->next;
                continue;
            }
            ok = true;
            colName = xmlGetProp(node, attr->name);
            attr = attr->next;
        }
        if (ok) {
            colValue = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (colName && colValue)
                InsertColor((const char*)colName, (const char*)colValue);
        }
        if (colName)
            xmlFree(colName);
        if (colValue)
            xmlFree(colValue);
        node = node->next;
    }
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

void cXmlParser::ParseGlobalVariables(xmlNodePtr node) {
    if (!node)
        return;
    
    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (xmlStrcmp(node->name, (const xmlChar *) "var")) {
            node = node->next;
            continue;
        }
        xmlAttrPtr attr = node->properties;
        if (attr == NULL) {
            node = node->next;
            continue;
        }
        xmlChar *varName = NULL;
        xmlChar *varType = NULL;
        xmlChar *varValue = NULL;
        while (NULL != attr) {
            if (!xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                varName = xmlGetProp(node, attr->name);
            } else if (!xmlStrcmp(attr->name, (const xmlChar *) "type")) {
                varType = xmlGetProp(node, attr->name);
            } else {
                attr = attr->next;
                continue;               
            }
            attr = attr->next;
        }
        varValue = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
        if (varName && varType && varValue)
            InsertVariable((const char*)varName, (const char*)varType, (const char*)varValue);
        if (varName)
            xmlFree(varName);
        if (varType)
            xmlFree(varType);
        if (varValue)
            xmlFree(varValue);
        node = node->next;
    }
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

void cXmlParser::ParseGlobalFonts(xmlNodePtr node) {
    if (!node)
        return;
    
    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (xmlStrcmp(node->name, (const xmlChar *) "font")) {
            node = node->next;
            continue;
        }
        xmlAttrPtr attr = node->properties;
        if (attr == NULL) {
            node = node->next;
            continue;
        }
        xmlChar *fontName = NULL;
        xmlChar *fontValue = NULL;
        bool ok = false;
        while (NULL != attr) {
            if (xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                attr = attr->next;
                continue;
            }
            ok = true;
            fontName = xmlGetProp(node, attr->name);
            attr = attr->next;
        }
        if (ok) {
            fontValue = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
            if (fontName && fontValue) {
                string fN = (const char*)fontName;
                string fV = (const char*)fontValue;
                globals->AddFont(fN, fV);
            }
        }
        if (fontName)
            xmlFree(fontName);
        if (fontValue)
            xmlFree(fontValue);
        node = node->next;
    }
}

void cXmlParser::ParseTranslations(xmlNodePtr node) {
    if (!node)
        return;
    
    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (xmlStrcmp(node->name, (const xmlChar *) "token")) {
            node = node->next;
            continue;
        }
        xmlAttrPtr attr = node->properties;
        if (attr == NULL) {
            node = node->next;
            continue;
        }
        xmlChar *tokenName;
        bool ok = false;
        while (NULL != attr) {
            if (xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                attr = attr->next;
                continue;
            }
            ok = true;
            tokenName = xmlGetProp(node, attr->name);
            attr = attr->next;
        }
        if (!ok)
            continue;
        map < string, string > tokenTranslations;
        xmlNodePtr nodeTrans = node->xmlChildrenNode;
        while (nodeTrans != NULL) {
            if (nodeTrans->type != XML_ELEMENT_NODE) {
                nodeTrans = nodeTrans->next;
                continue;
            }
            xmlChar *language = NULL;
            if (xmlStrcmp(nodeTrans->name, (const xmlChar *) "trans")) {
                nodeTrans = nodeTrans->next;
                continue;
            }
            xmlAttrPtr attrTrans = nodeTrans->properties;
            if (attrTrans == NULL) {
                nodeTrans = nodeTrans->next;
                continue;
            }
            ok = false;
            
            while (NULL != attrTrans) {
                if (!ok && xmlStrcmp(attrTrans->name, (const xmlChar *) "lang")) {
                    attrTrans = attrTrans->next;
                    continue;
                }
                ok = true;
                language = xmlGetProp(nodeTrans, attrTrans->name);
                attrTrans = attrTrans->next;
            }
            if (!ok)
                continue;
            xmlChar *value = NULL;
            value = xmlNodeListGetString(doc, nodeTrans->xmlChildrenNode, 1);
            if (language && value)
                tokenTranslations.insert(pair<string, string>((const char*)language, (const char*)value));
            if (language)
                xmlFree(language);
            if (value)
                xmlFree(value);
            nodeTrans = nodeTrans->next;
        }
        if (globals) {
            globals->AddTranslation((const char*)tokenName, tokenTranslations);
        } else if (skinSetup) {
            skinSetup->SetTranslation((const char*)tokenName, tokenTranslations);
        }
        xmlFree(tokenName);
        node = node->next;
    }
}

bool cXmlParser::ParseSubView(xmlNodePtr node) {
    if (!node)
        return false;
    
    if (!view)
        return false;

    cTemplateView *subView = new cTemplateViewMenu();
    view->AddSubView((const char*)node->name, subView);

    vector<pair<string, string> > subViewAttribs;
    ParseAttributes(node->properties, node, subViewAttribs);

    subView->SetParameters(subViewAttribs);

    xmlNodePtr childNode = node->xmlChildrenNode;

    while (childNode != NULL) {

        if (childNode->type != XML_ELEMENT_NODE) {
            childNode = childNode->next;
            continue;
        }

        if (subView->ValidViewElement((const char*)childNode->name)) {
            xmlAttrPtr attr = childNode->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, childNode, attribs, true);
            ParseViewElement(childNode->name, childNode->xmlChildrenNode, attribs, subView);
        } else if (subView->ValidViewList((const char*)childNode->name)) {
            ParseViewList(childNode, subView);
        } else if (!xmlStrcmp(childNode->name, (const xmlChar *) "tab")) {
            ParseViewTab(childNode, subView);           
        } else {
            return false;
        }

        childNode = childNode->next;
    }

    

    return true;

}

void cXmlParser::ParseViewElement(const xmlChar * viewElement, xmlNodePtr node, vector<pair<string, string> > &attributes, cTemplateView *subView) {
    if (!node)
        return;
    
    if (!view)
        return;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }

        if (xmlStrcmp(node->name, (const xmlChar *) "area") && xmlStrcmp(node->name, (const xmlChar *) "areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\"", node->name);
            node = node->next;
            continue;
        }

        xmlAttrPtr attr = node->properties;
        vector<pair<string, string> > attribs;
        ParseAttributes(attr, node, attribs);

        cTemplatePixmap *pix = new cTemplatePixmap();
        if (!xmlStrcmp(node->name, (const xmlChar *) "areascroll")) {
            pix->SetScrolling();
        }
        pix->SetParameters(attribs);
        ParseFunctionCalls(node->xmlChildrenNode, pix);
        if (subView)
            subView->AddPixmap((const char*)viewElement, pix, attributes);
        else
            view->AddPixmap((const char*)viewElement, pix, attributes);
        
        node = node->next;
    }
}

void cXmlParser::ParseViewList(xmlNodePtr parentNode, cTemplateView *subView) {
    if (!parentNode || !view)
        return;
    
    xmlAttrPtr attr = parentNode->properties;
    vector<pair<string, string> > attribs;
    ParseAttributes(attr, parentNode, attribs);

    cTemplateViewList *viewList = new cTemplateViewList();
    viewList->SetGlobals(globals);
    viewList->SetParameters(attribs);

    xmlNodePtr node = parentNode->xmlChildrenNode;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }

        if (!xmlStrcmp(node->name, (const xmlChar *) "currentelement")) {
            xmlNodePtr childNode = node->xmlChildrenNode;
            if (!childNode)
                continue;
            cTemplateViewElement *currentElement = new cTemplateViewElement();
            xmlAttrPtr attrCur = node->properties;
            vector<pair<string, string> > attribsCur;
            ParseAttributes(attrCur, node, attribsCur);
            currentElement->SetGlobals(globals);
            currentElement->SetParameters(attribsCur);
            while (childNode != NULL) {
                if (childNode->type != XML_ELEMENT_NODE) {
                    childNode = childNode->next;
                    continue;
                }
                if ((!xmlStrcmp(childNode->name, (const xmlChar *) "area")) || (!xmlStrcmp(childNode->name, (const xmlChar *) "areascroll"))) {
                    xmlAttrPtr attrPix = childNode->properties;
                    vector<pair<string, string> > attribsPix;
                    ParseAttributes(attrPix, childNode, attribsPix);
                    cTemplatePixmap *pix = new cTemplatePixmap();
                    pix->SetParameters(attribsPix);
                    ParseFunctionCalls(childNode->xmlChildrenNode, pix);
                    if (!xmlStrcmp(childNode->name, (const xmlChar *) "areascroll")) {
                        pix->SetScrolling();
                    }
                    currentElement->AddPixmap(pix);
                }
                childNode = childNode->next;
            }
            viewList->AddCurrentElement(currentElement);
        } else if (!xmlStrcmp(node->name, (const xmlChar *) "listelement")) {
            xmlNodePtr childNode = node->xmlChildrenNode;
            if (!childNode)
                continue;
            cTemplateViewElement *listElement = new cTemplateViewElement();
            xmlAttrPtr attrList = node->properties;
            vector<pair<string, string> > attribsList;
            ParseAttributes(attrList, node, attribsList);
            listElement->SetGlobals(globals);
            listElement->SetParameters(attribsList);
            while (childNode != NULL) {
                if (childNode->type != XML_ELEMENT_NODE) {
                    childNode = childNode->next;
                    continue;
                }
                if ((!xmlStrcmp(childNode->name, (const xmlChar *) "area")) || (!xmlStrcmp(childNode->name, (const xmlChar *) "areascroll"))) {
                    xmlAttrPtr attrPix = childNode->properties;
                    vector<pair<string, string> > attribsPix;
                    ParseAttributes(attrPix, childNode, attribsPix);
                    cTemplatePixmap *pix = new cTemplatePixmap();
                    pix->SetParameters(attribsPix);
                    ParseFunctionCalls(childNode->xmlChildrenNode, pix);
                    if (!xmlStrcmp(childNode->name, (const xmlChar *) "areascroll")) {
                        pix->SetScrolling();
                    }
                    listElement->AddPixmap(pix);
                }
                childNode = childNode->next;
            }
            viewList->AddListElement(listElement);
        } else {
            node = node->next;
            continue;
        }
        node = node->next;
    }
    if (subView)
        subView->AddViewList((const char*)parentNode->name, viewList);
    else
        view->AddViewList((const char*)parentNode->name, viewList);
}

void cXmlParser::ParseViewTab(xmlNodePtr parentNode, cTemplateView *subView) {
    if (!parentNode || !view || !subView)
        return;

    xmlAttrPtr attr = parentNode->properties;
    vector<pair<string, string> > attribs;
    ParseAttributes(attr, parentNode, attribs);

    cTemplateViewTab *viewTab = new cTemplateViewTab();
    viewTab->SetGlobals(globals);
    viewTab->SetParameters(attribs);
    viewTab->SetScrolling();
    xmlNodePtr node = parentNode->xmlChildrenNode;
    ParseFunctionCalls(node, viewTab);

    subView->AddViewTab(viewTab);
}

void cXmlParser::ParseGrid(xmlNodePtr node, vector<pair<string, string> > &attributes) {
    if (!node)
        return;
    
    if (!view)
        return;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }

        if (xmlStrcmp(node->name, (const xmlChar *) "area") && xmlStrcmp(node->name, (const xmlChar *) "areascroll")) {
            esyslog("skindesigner: invalid tag \"%s\"", node->name);
            node = node->next;
            continue;
        }

        xmlAttrPtr attr = node->properties;
        vector<pair<string, string> > attribs;
        ParseAttributes(attr, node, attribs);

        cTemplatePixmap *pix = new cTemplatePixmap();
        if (!xmlStrcmp(node->name, (const xmlChar *) "areascroll")) {
            pix->SetScrolling();
        }
        pix->SetParameters(attribs);
        ParseFunctionCalls(node->xmlChildrenNode, pix);
        view->AddPixmapGrid(pix, attributes);
        
        node = node->next;
    }
}

void cXmlParser::ParseFunctionCalls(xmlNodePtr node, cTemplatePixmap *pix) {
    if (!node)
        return;

    if (!view)
        return;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }

        if (!xmlStrcmp(node->name, (const xmlChar *) "loop")) {
            xmlNodePtr childNode = node->xmlChildrenNode;
            if (!childNode)
                continue;
            xmlAttrPtr attr = node->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, node, attribs);
            cTemplateLoopFunction *loopFunc = new cTemplateLoopFunction();
            loopFunc->SetParameters(attribs);
            ParseLoopFunctionCalls(childNode, loopFunc);
            pix->AddLoopFunction(loopFunc);
            node = node->next;
        } else if (view->ValidFunction((const char*)node->name)) {
            xmlAttrPtr attr = node->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, node, attribs);
            pix->AddFunction((const char*)node->name, attribs);
            node = node->next;
        } else {
            node = node->next;
            continue;           
        }

    }
}

void cXmlParser::ParseLoopFunctionCalls(xmlNodePtr node, cTemplateLoopFunction *loopFunc) {
    if (!node)
        return;

    if (!view)
        return;

    while (node != NULL) {

        if (node->type != XML_ELEMENT_NODE) {
            node = node->next;
            continue;
        }
        if (view->ValidFunction((const char*)node->name)) {
            xmlAttrPtr attr = node->properties;
            vector<pair<string, string> > attribs;
            ParseAttributes(attr, node, attribs);
            loopFunc->AddFunction((const char*)node->name, attribs);
            node = node->next;
        } else {
            node = node->next;
            continue;           
        }

    }
}

bool cXmlParser::ParseAttributes(xmlAttrPtr attr, xmlNodePtr node, vector<pair<string, string> > &attribs, bool isViewElement) {
    if (attr == NULL) {
        return false;
    }

    if (!view)
        return false;

    while (NULL != attr) {

        string name = (const char*)attr->name;
        xmlChar *value = NULL;
        value = xmlGetProp(node, attr->name);
        if (!view->ValidAttribute(isViewElement ? "viewelement" : (const char*)node->name, (const char*)attr->name)) {
            esyslog("skindesigner: unknown attribute %s in %s", (const char*)attr->name, (const char*)node->name);
            attr = attr->next;
            if (value)
                xmlFree(value);
            continue;
        }
        if (value)
            attribs.push_back(pair<string, string>((const char*)attr->name, (const char*)value));
        attr = attr->next;
        if (value)
            xmlFree(value);
    }
    return true;
}

void cXmlParser::InitLibXML() {
    xmlInitParser();
}

void cXmlParser::CleanupLibXML() {
    xmlCleanupParser();
}
