#include "../libcore/helpers.h"
#include "libxmlwrapper.h"

void SkinDesignerXMLErrorHandler (void * userData, xmlErrorPtr error) {
    esyslog("skindesigner: Error in XML: %s", error->message);
}

cLibXMLWrapper::cLibXMLWrapper(void) {
    ctxt = NULL;
    doc = NULL;
    root = NULL;

    initGenericErrorDefaultFunc(NULL);
    xmlSetStructuredErrorFunc(NULL, SkinDesignerXMLErrorHandler);
    ctxt = xmlNewParserCtxt();
}

cLibXMLWrapper::~cLibXMLWrapper() {
    DeleteDocument();
    xmlFreeParserCtxt(ctxt);
}

void cLibXMLWrapper::InitLibXML() {
    xmlInitParser();
}

void cLibXMLWrapper::CleanupLibXML() {
    xmlCleanupParser();
}

void cLibXMLWrapper::DeleteDocument(void) {
    if (doc) {
        xmlFreeDoc(doc);
        doc = NULL;
    }
    while (!nodeStack.empty())
        nodeStack.pop();
}

bool cLibXMLWrapper::ReadXMLFile(const char *path, bool validate) {
    if (!ctxt) {
        esyslog("skindesigner: Failed to allocate parser context");
        return false;
    }
    if (!FileExists(path)) {
        dsyslog("skindesigner: reading XML Template %s failed", path);
        return false;
    }
    if (validate)
        doc = xmlCtxtReadFile(ctxt, path, NULL, XML_PARSE_NOENT | XML_PARSE_DTDVALID);
    else
        doc = xmlCtxtReadFile(ctxt, path, NULL, XML_PARSE_NOENT);

    if (!doc) {
        dsyslog("skindesigner: reading XML Template %s failed", path);
        return false;
    }
    return true;
}

bool cLibXMLWrapper::SetDocument(void) {
    if (!doc)
        return false;
    root = xmlDocGetRootElement(doc);
    nodeStack.push(root);
    if (root == NULL) {
        esyslog("skindesigner: ERROR: XML File is empty");
        return false;
    }
    return true;
}

bool cLibXMLWrapper::Validate(void) {
    if (!ctxt)
        return false;
    if (ctxt->valid == 0) {
        esyslog("skindesigner: Failed to validate XML File");
        return false;
    }
    return true;
}

bool cLibXMLWrapper::CheckNodeName(const char *name) {
    if (nodeStack.empty())
        return false;
    xmlNodePtr current = nodeStack.top(); 
    if (xmlStrcmp(current->name, (const xmlChar *) name)) {
        return false;
    }
    return true;
}

const char *cLibXMLWrapper::NodeName(void) {
    xmlNodePtr current = nodeStack.top(); 
    return (const char*)current->name;
}

vector<stringpair> cLibXMLWrapper::ParseAttributes(void) {
    vector<stringpair> attributes;
    if (nodeStack.empty())
        return attributes;
    xmlNodePtr current = nodeStack.top();
    xmlAttrPtr attrPtr = current->properties;
    if (attrPtr == NULL) {
        return attributes;
    }
    while (NULL != attrPtr) {
        string name = (const char*)attrPtr->name;
        xmlChar *value = NULL;
        value = xmlGetProp(current, attrPtr->name);
        if (value)
            attributes.push_back(stringpair((const char*)attrPtr->name, (const char*)value));
        attrPtr = attrPtr->next;
        if (value)
            xmlFree(value);
    }
    return attributes;
}

bool cLibXMLWrapper::LevelDown(void) {
    if (nodeStack.empty())
        return false;
    xmlNodePtr current = nodeStack.top();
    xmlNodePtr child = current->xmlChildrenNode;
    while (child && child->type != XML_ELEMENT_NODE) {
        child = child->next;
    }
    if (!child)
        return false;
    nodeStack.push(child);
    return true;
}

bool cLibXMLWrapper::LevelUp(void) {
    if (nodeStack.size() == 1)
        return false;
    nodeStack.pop();
    return true;
}

bool cLibXMLWrapper::NextNode(void) {
    xmlNodePtr current = nodeStack.top();
    current = current->next;
    while (current && current->type != XML_ELEMENT_NODE) {
        current = current->next;
    }
    if (!current)
        return false;
    nodeStack.pop();
    nodeStack.push(current);
    return true;
}

bool cLibXMLWrapper::GetAttribute(string &name, string &value) {
    bool ok = false;
    xmlNodePtr current = nodeStack.top();
    xmlAttrPtr attr = current->properties;
    if (attr == NULL) {
        return ok;
    }
    xmlChar *xmlValue = NULL;
    while (NULL != attr) {
        if (xmlStrcmp(attr->name, (const xmlChar *) name.c_str())) {
            attr = attr->next;
            continue;
        }
        ok = true;
        xmlValue = xmlGetProp(current, attr->name);
        if (xmlValue) {
            value = (const char*)xmlValue;
            xmlFree(xmlValue);
        }
        break;
    }
    return ok;
}

bool cLibXMLWrapper::GetNodeValue(string &value) {
    xmlChar *val = NULL;
    xmlNodePtr current = nodeStack.top();
    val = xmlNodeListGetString(doc, current->xmlChildrenNode, 1);
    if (val) {
        value = (const char*)val;
        xmlFree(val);
        return true;
    }
    value = "";
    return false;
}
