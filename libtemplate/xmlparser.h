#ifndef __XMLPARSER_H
#define __XMLPARSER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <vdr/plugin.h>
#include "../libcore/libxmlwrapper.h"
#include "templateview.h"
#include "templateviewlist.h"
#include "templateviewgrid.h"
#include "templateviewtab.h"
#include "../libcore/skinsetup.h"

using namespace std;

// --- cXmlParser -------------------------------------------------------------

class cXmlParser : public cLibXMLWrapper {
private:
    cTemplateView *view;
    cGlobals *globals;
    cSkinSetup *skinSetup;
    //parsing views
    bool ParseSubView(void);
    void ParseViewElement(cTemplateView *subView = NULL);
    void ParseViewList(cTemplateView *subView = NULL);
    void ParseViewTab(cTemplateView *subView);
    void ParseGrid(void);
    cTemplatePixmap *ParseArea(void);
    cTemplatePixmapContainer *ParseAreaContainer(void);
    void ParseFunctionCalls(cTemplatePixmap *pix);
    void ParseLoopFunctionCalls(cTemplateLoopFunction *loopFunc);
    //parsing globals
    void ParseGlobalColors(void);
    void InsertColor(string name, string value);
    void ParseGlobalVariables(void);
    void InsertVariable(string name, string type, string value);
    void ParseGlobalFonts(void);
    void ParseTranslations(void);
    //parsing skin setup
    void ParseSetupMenu(void);
    void ParseSetupParameter(void);
    //helpers
    void ValidateAttributes(const char *nodeName, vector<stringpair> &attributes);
    string GetPath(string xmlFile);
public:
    cXmlParser(void);
    virtual ~cXmlParser(void);
    //reading views
    bool ReadView(cTemplateView *view, string xmlFile);
    bool ParseView(void);
    //reading plugin views
    bool ReadPluginView(string plugName, int templateNumber, string templateName);
    bool ParsePluginView(string plugName, int templateNumber);
    //reading globals
    bool ReadGlobals(cGlobals *globals, string xmlFile);
    bool ParseGlobals(void);
    //reading skin setups
    bool ReadSkinSetup(cSkinSetup *skinSetup, string xmlFile);
    bool ParseSkinSetup(string skin);
};

#endif //__XMLPARSER_H
