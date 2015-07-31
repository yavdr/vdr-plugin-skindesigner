#ifndef __LIBXMLWRAPPER_H
#define __LIBXMLWRAPPER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlerror.h>
#include <vdr/plugin.h>

using namespace std;

#include "../views/viewhelpers.h"

class cLibXMLWrapper {
private:
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;
    xmlNodePtr root;
    xmlNodePtr current;
    stack<xmlNodePtr> nodeStack;
protected:
    void DeleteDocument(void);
    bool ReadXMLFile(const char *path, bool validate = true);
    bool SetDocument(void);
    bool Validate(void);
    bool CheckNodeName(const char *name);
    const char *NodeName(void);
    vector<stringpair> ParseAttributes(void);
    bool LevelDown(void);
    bool LevelUp(void);
    bool NextNode(void);
    bool GetAttribute(string &name, string &value);
    bool GetNodeValue(string &value);
public:
    cLibXMLWrapper(void);
    virtual ~cLibXMLWrapper(void);
    static void InitLibXML();
    static void CleanupLibXML();
};

#endif //__LIBXMLWRAPPER_H