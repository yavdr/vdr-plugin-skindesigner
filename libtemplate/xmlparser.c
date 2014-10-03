#include "xmlparser.h"
#include "../config.h"

using namespace std;

void SkinDesignerXMLErrorHandler (void * userData, xmlErrorPtr error) {
    esyslog("skindesigner: Error in XML: %s", error->message);
}

cXmlParser::cXmlParser(void) {
    doc = NULL;
    root = NULL;
    ctxt = NULL;

    xmlInitParser();
    initGenericErrorDefaultFunc(NULL);
    xmlSetStructuredErrorFunc(NULL, SkinDesignerXMLErrorHandler);
    ctxt = xmlNewParserCtxt();
}

cXmlParser::~cXmlParser() {
    DeleteDocument();
    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
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

bool cXmlParser::ReadGlobals(cGlobals *globals, string xmlFile) {
    this->globals = globals;
    
    string xmlPath = GetPath(xmlFile);

    if (ctxt == NULL) {
        esyslog("skindesigner: Failed to allocate parser context");
        return false;
    }
    
    doc = xmlCtxtReadFile(ctxt, xmlPath.c_str(), NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);

    if (doc == NULL ) {
        esyslog("skindesigner: ERROR: Globals %s not parsed successfully.", xmlPath.c_str());
        return false;
    }

    root = xmlDocGetRootElement(doc);

    if (ctxt->valid == 0) {
        esyslog("skindesigner: Failed to validate %s", xmlPath.c_str());
        return false;
    }

    if (root == NULL) {
        esyslog("skindesigner: ERROR: Globals %s is empty", xmlPath.c_str());
        return false;
    }

    if (xmlStrcmp(root->name, (const xmlChar *) "globals")) {
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
            bool debugViewElement = DebugViewElement(node);
            ParseViewElement(node->name, node->xmlChildrenNode, debugViewElement);
        } else if (view->ValidViewList((const char*)node->name)) {
            ParseViewList(node);
        } else {
            return false;
        }

        node = node->next;
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
        path = *cString::sprintf("%s%s/themes/%s/%s", *config.skinPath, activeSkin.c_str(), activeTheme.c_str(), xmlFile.c_str());
    } else {
        path = *cString::sprintf("%s%s/xmlfiles/%s", *config.skinPath, activeSkin.c_str(), xmlFile.c_str());
    }
    return path;
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
    globals->colors.insert(pair<string, tColor>(name, colVal));
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
        globals->intVars.insert(pair<string, int>(name, val));
    } else if (!type.compare("string")) {
        globals->stringVars.insert(pair<string, string>(name, value));
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
            if (fontName && fontValue)
                globals->fonts.insert(pair<string, string>((const char*)fontName, (const char*)fontValue));
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
        globals->translations.insert(pair<string, map < string, string > >((const char*)tokenName, tokenTranslations));
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
            bool debugViewElement = DebugViewElement(childNode);
            ParseViewElement(childNode->name, childNode->xmlChildrenNode, debugViewElement, subView);
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

void cXmlParser::ParseViewElement(const xmlChar * viewElement, xmlNodePtr node, bool debugVE, cTemplateView *subView) {
    if (!node)
        return;
    
    if (!view)
        return;

    if (debugVE) {
        dsyslog("skindesigner: activating debugging of viewElement %s", (const char*)viewElement);
    }

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
            subView->AddPixmap((const char*)viewElement, pix, debugVE);
        else
            view->AddPixmap((const char*)viewElement, pix, debugVE);
        
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
            bool debugCurrent = false;
            for (vector<pair<string, string> >::iterator it = attribsCur.begin(); it != attribsCur.end(); it++) {
                if (!(it->first).compare("debug")) {
                    debugCurrent = true;
                    break;
                }
            }
            if (debugCurrent)
                currentElement->ActivateDebugTokens();
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
            bool debugViewList = DebugViewElement(node);
            xmlNodePtr childNode = node->xmlChildrenNode;
            if (!childNode)
                continue;
            cTemplateViewElement *listElement = new cTemplateViewElement();
            if (debugViewList)
                listElement->ActivateDebugTokens();
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

bool cXmlParser::ParseAttributes(xmlAttrPtr attr, xmlNodePtr node, vector<pair<string, string> > &attribs) {
    if (attr == NULL) {
        return false;
    }

    if (!view)
        return false;

    while (NULL != attr) {

        string name = (const char*)attr->name;
        if (!name.compare("debug")) {
            attribs.push_back(pair<string, string>((const char*)attr->name, "true"));
            attr = attr->next;
            continue;
        }

        xmlChar *value = NULL;
        value = xmlGetProp(node, attr->name);
        if (!view->ValidAttribute((const char*)node->name, (const char*)attr->name)) {
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

bool cXmlParser::DebugViewElement(xmlNodePtr node) {
    xmlAttrPtr attr = node->properties;
    vector<pair<string, string> > attribs;
    ParseAttributes(attr, node, attribs);
    for (vector<pair<string, string> >::iterator it = attribs.begin(); it != attribs.end(); it++) {
        if (!(it->first).compare("debug"))
            return true;
    }
    return false;
}
