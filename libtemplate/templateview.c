#include "../config.h"
#include "templateview.h"

// --- cTemplateView -------------------------------------------------------------

cTemplateView::cTemplateView(void) {
    globals = NULL;
    parameters = NULL;
    containerX = 0;
    containerY = 0;
    containerWidth = 0;
    containerHeight = 0;
    SetFunctionDefinitions();
}

cTemplateView::~cTemplateView() {

    for (map < eViewElement, cTemplateViewElement* >::iterator it = viewElements.begin(); it != viewElements.end(); it++) {
        delete it->second;
    }

    for (map < eViewList, cTemplateViewList* >::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        delete it->second;
    }

    for (map < int, cTemplateViewGrid* >::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
        delete it->second;
    }

    for (vector < cTemplateViewTab* >::iterator it = viewTabs.begin(); it != viewTabs.end(); it++) {
        delete *it;
    }

    for (map < eSubView, cTemplateView* >::iterator it = subViews.begin(); it != subViews.end(); it++) {
        delete it->second;
    }

    for (map < string, map< int, cTemplateView*> >::iterator it = pluginViews.begin(); it != pluginViews.end(); it++) {
        map< int, cTemplateView*> plugViews = it->second;
        for (map< int, cTemplateView*>::iterator it2 = plugViews.begin(); it2 != plugViews.end(); it2++) {
            delete it2->second;
        }
    }

    if (parameters)
        delete parameters;

}

/*******************************************************************
* Public Functions
*******************************************************************/

void cTemplateView::SetParameters(vector<pair<string, string> > &params) {
    parameters = new cTemplateFunction(ftView);
    parameters->SetGlobals(globals);
    parameters->SetParameters(params);
}

void cTemplateView::SetContainer(int x, int y, int width, int height) {
    containerX = x;
    containerY = y;
    containerWidth = width; 
    containerHeight = height; 
}

cTemplateViewElement *cTemplateView::GetViewElement(eViewElement ve) {
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        return NULL;
    }
    return hit->second;
}

void cTemplateView::InitViewElementIterator(void) {
    veIt = viewElements.begin();
}

cTemplateViewElement *cTemplateView::GetNextViewElement(void) {
    if (veIt == viewElements.end())
        return NULL;
    cTemplateViewElement *viewElement = veIt->second;
    veIt++;
    return viewElement; 
}

cTemplateViewGrid *cTemplateView::GetViewGrid(int gridID) {
    map < int, cTemplateViewGrid* >::iterator hit = viewGrids.find(gridID);
    if (hit == viewGrids.end()) {
        return NULL;
    }
    return hit->second;    
}

void cTemplateView::InitViewGridIterator(void) {
    geIt = viewGrids.begin();
}

cTemplateViewGrid *cTemplateView::GetNextViewGrid(void) {
    if (geIt == viewGrids.end())
        return NULL;
    cTemplateViewGrid *viewGrid = geIt->second;
    geIt++;
    return viewGrid; 
}

cTemplateViewList *cTemplateView::GetViewList(eViewList vl) {
    map < eViewList, cTemplateViewList* >::iterator hit = viewLists.find(vl);
    if (hit == viewLists.end())
        return NULL;
    return hit->second;
}

void cTemplateView::InitViewListIterator(void) {
    vlIt = viewLists.begin();
}

cTemplateViewList *cTemplateView::GetNextViewList(void) {
    if (vlIt == viewLists.end())
        return NULL;
    cTemplateViewList *viewList = vlIt->second;
    vlIt++;
    return viewList;    
}

cTemplateView *cTemplateView::GetSubView(eSubView sv) {
    map < eSubView, cTemplateView* >::iterator hit = subViews.find(sv);
    if (hit == subViews.end())
        return NULL;
    return hit->second;
}

cTemplateView *cTemplateView::GetPluginView(string pluginName, int pluginMenu) {
    map < string, map< int, cTemplateView*> >::iterator hit = pluginViews.find(pluginName);

    if (hit == pluginViews.end())
       return NULL;

    map< int, cTemplateView*> plugViews = hit->second;
    map< int, cTemplateView*>::iterator hit2 = plugViews.find(pluginMenu);

    if (hit2 == plugViews.end())
        return NULL;

    return hit2->second;
}


void cTemplateView::InitViewTabIterator(void) {
    vtIt = viewTabs.begin();
}

cTemplateViewTab *cTemplateView::GetNextViewTab(void) {
    if (vtIt == viewTabs.end()) {
        return NULL;
    }
    cTemplateViewTab *tab = *vtIt;
    vtIt++;
    return tab;
}

void cTemplateView::InitSubViewIterator(void) {
    svIt = subViews.begin();
}

cTemplateView *cTemplateView::GetNextSubView(void) {
    if (svIt == subViews.end())
        return NULL;
    cTemplateView *subView = svIt->second;
    svIt++;
    return subView; 
}

int cTemplateView::GetNumericParameter(eParamType type) {
    if (!parameters)
        return 0;
    return parameters->GetNumericParameter(type);
}

cRect cTemplateView::GetOsdSize(void) {
    cRect osdSize;
    if (!parameters) {
        return osdSize;
    }
    osdSize.SetX(parameters->GetNumericParameter(ptX));
    osdSize.SetY(parameters->GetNumericParameter(ptY));
    osdSize.SetWidth(parameters->GetNumericParameter(ptWidth));
    osdSize.SetHeight(parameters->GetNumericParameter(ptHeight));
    return osdSize;
}

int cTemplateView::GetNumPixmaps(void) {
    int numPixmaps = 0;
    for (map < eViewElement, cTemplateViewElement* >::iterator it = viewElements.begin(); it != viewElements.end(); it++) {
        cTemplateViewElement *viewElement = it->second;
        numPixmaps += viewElement->GetNumPixmaps();
    }
    return numPixmaps;
}

int cTemplateView::GetNumPixmapsViewElement(eViewElement ve) {
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end())
        return 0;
    cTemplateViewElement *viewElement = hit->second; 
    return viewElement->GetNumPixmaps();
}

bool cTemplateView::HideView(void) {
    if (!parameters)
        return false;
    return parameters->GetNumericParameter(ptHideRoot);
}

bool cTemplateView::DrawGebugGrid(void) {
    if (!parameters)
        return false;
    return parameters->GetNumericParameter(ptDrawDebugGrid);
}

int cTemplateView::DebugGridX(void) {
    if (!parameters)
        return 0;
    return parameters->GetNumericParameter(ptDebugGridX);
}

int cTemplateView::DebugGridY(void) {
    if (!parameters)
        return 0;
    return parameters->GetNumericParameter(ptDebugGridY);
}

tColor cTemplateView::DebugGridColor(void) {
    tColor col = 0xFFFF0000;
    if (!globals)
        return col;
    string colName = "{clrDebugGrid}";
    globals->GetColor(colName, col);
    return col;
}

tColor cTemplateView::DebugGridFontColor(void) {
    tColor col = 0xFFFF0000;
    if (!globals)
        return col;
    string colName = "{clrFontDebugGrid}";
    globals->GetColor(colName, col);
    return col;
}

bool cTemplateView::ExecuteView(eViewElement ve) {
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end())
        return false;
    cTemplateViewElement *viewElement = hit->second;
    return viewElement->Execute();
}

bool cTemplateView::DetachViewElement(eViewElement ve) {
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end())
        return false;
    cTemplateViewElement *viewElement = hit->second;
    return viewElement->Detach();    
}

string cTemplateView::GetViewElementMode(eViewElement ve) {
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end())
        return "";
    cTemplateViewElement *viewElement = hit->second;
    return viewElement->GetMode();    
}

int cTemplateView::GetNumListViewMenuItems(void) {
    int numElements = 0;
    cTemplateViewList *menuList = GetViewList(vlMenuItem);
    if (!menuList)
        return numElements;
    return menuList->GetNumericParameter(ptNumElements);
}

bool cTemplateView::GetScalingWindow(cRect &scalingWindow) {
    if (!parameters)
        return false;
    bool doScale = false;
    int scaleX = parameters->GetNumericParameter(ptScaleTvX) + cOsd::OsdLeft();
    int scaleY = parameters->GetNumericParameter(ptScaleTvY) + cOsd::OsdTop();
    int scaleWidth = parameters->GetNumericParameter(ptScaleTvWidth);
    int scaleHeight = parameters->GetNumericParameter(ptScaleTvHeight);
    if (scaleX > -1 && scaleY > -1 && scaleWidth > -1 && scaleHeight > -1) {
        cRect suggestedScaleWindow(scaleX, scaleY, scaleWidth, scaleHeight);
        scalingWindow = cDevice::PrimaryDevice()->CanScaleVideo(suggestedScaleWindow);
        doScale = true;
    } else {
        scalingWindow = cDevice::PrimaryDevice()->CanScaleVideo(cRect::Null);
    }
    return doScale;
}

bool cTemplateView::ValidViewElement(const char *viewElement) {
    set<string>::iterator hit = viewElementsAllowed.find(viewElement);
    if (hit == viewElementsAllowed.end())
        return false;
    return true;
}

bool cTemplateView::ValidSubView(const char *subView) {
    set<string>::iterator hit = subViewsAllowed.find(subView);
    if (hit == subViewsAllowed.end())
        return false;
    return true;        
}

bool cTemplateView::ValidViewList(const char *viewList) {
    set<string>::iterator hit = viewListsAllowed.find(viewList);
    if (hit == viewListsAllowed.end())
        return false;
    return true;
}

bool cTemplateView::ValidViewGrid(const char *viewGrid) {
    set<string>::iterator hit = viewGridsAllowed.find(viewGrid);
    if (hit == viewGridsAllowed.end())
        return false;
    return true;
}

bool cTemplateView::ValidFunction(const char *func) {
    map < string, set < string > >::iterator hit = funcsAllowed.find(func);
    if (hit == funcsAllowed.end())
        return false;
    return true;
}

bool cTemplateView::ValidAttribute(const char *func, const char *att) {
    map < string, set < string > >::iterator hit = funcsAllowed.find(func);
    if (hit == funcsAllowed.end())
        return false;
    
    set<string>::iterator hitAtt = (hit->second).find(att);
    if (hitAtt == (hit->second).end())
        return false;

    return true;
}

void cTemplateView::Translate(void) {
    //Translate ViewElements
    InitViewElementIterator();
    cTemplateViewElement *viewElement = NULL;
    while(viewElement = GetNextViewElement()) {
        viewElement->InitIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewElement->GetNextPixmap()) {
            pix->InitIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                    string text = func->GetParameter(ptText);
                    string translation;
                    bool translated = globals->Translate(text, translation);
                    if (translated) {
                        func->SetTranslatedText(translation);
                    }
                }
                if (func->GetType() == ftLoop) {
                    cTemplateLoopFunction *funcsLoop =  dynamic_cast<cTemplateLoopFunction*>(func);
                    funcsLoop->InitIterator();
                    cTemplateFunction *loopFunc = NULL;
                    while(loopFunc = funcsLoop->GetNextFunction()) {
                        if (loopFunc->GetType() == ftDrawText || loopFunc->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                            string text = loopFunc->GetParameter(ptText);
                            string translation;
                            bool translated = globals->Translate(text, translation);
                            if (translated) {
                                loopFunc->SetTranslatedText(translation);
                            }
                        }
                    }
                }
            }
        }
    }
    //Translate viewLists
    InitViewListIterator();
    cTemplateViewList *viewList = NULL;
    while(viewList = GetNextViewList()) {
        viewList->InitIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewList->GetNextPixmap()) {
            pix->InitIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                    string text = func->GetParameter(ptText);
                    string translation;
                    bool translated = globals->Translate(text, translation);
                    if (translated) {
                        func->SetTranslatedText(translation);
                    }
                }
            }
        }
        cTemplateViewElement *listElement = viewList->GetListElement();
        listElement->InitIterator();
        while(pix = listElement->GetNextPixmap()) {
            pix->InitIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                    string text = func->GetParameter(ptText);
                    string translation;
                    bool translated = globals->Translate(text, translation);
                    if (translated) {
                        func->SetTranslatedText(translation);
                    }
                }
            }
        }
        
        cTemplateViewElement *listElementCurrent = viewList->GetListElementCurrent();
        if (listElementCurrent) {
            listElementCurrent->InitIterator();
            while(pix = listElementCurrent->GetNextPixmap()) {
                pix->InitIterator();
                cTemplateFunction *func = NULL;
                while(func = pix->GetNextFunction()) {
                    if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                        string text = func->GetParameter(ptText);
                        string translation;
                        bool translated = globals->Translate(text, translation);
                        if (translated) {
                            func->SetTranslatedText(translation);
                        }
                    }
                }
            }
        }
    }

    //Translate viewTabs
    InitViewTabIterator();
    cTemplateViewTab *viewTab = NULL;
    while(viewTab = GetNextViewTab()) {
        string tabName = viewTab->GetName();
        string tabTrans;
        bool translated = globals->Translate(tabName, tabTrans);
        if (translated) {
            viewTab->SetName(tabTrans);            
        }
        viewTab->InitIterator();
        cTemplateFunction *func = NULL;
        while(func = viewTab->GetNextFunction()) {
            if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                string text = func->GetParameter(ptText);
                string translation;
                translated = globals->Translate(text, translation);
                if (translated) {
                    func->SetTranslatedText(translation);
                }
            }
            if (func->GetType() == ftLoop) {
                cTemplateLoopFunction *funcsLoop =  dynamic_cast<cTemplateLoopFunction*>(func);
                funcsLoop->InitIterator();
                cTemplateFunction *loopFunc = NULL;
                while(loopFunc = funcsLoop->GetNextFunction()) {
                    if (loopFunc->GetType() == ftDrawText || loopFunc->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                        string text = loopFunc->GetParameter(ptText);
                        string translation;
                        bool translated = globals->Translate(text, translation);
                        if (translated) {
                            loopFunc->SetTranslatedText(translation);
                        }
                    }
                }
            }
        }
    }

    //Translate ViewGrids
    InitViewGridIterator();
    cTemplateViewGrid *viewGrid = NULL;
    while(viewGrid = GetNextViewGrid()) {
        viewGrid->InitIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewGrid->GetNextPixmap()) {
            pix->InitIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText || func->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                    string text = func->GetParameter(ptText);
                    string translation;
                    bool translated = globals->Translate(text, translation);
                    if (translated) {
                        func->SetTranslatedText(translation);
                    }
                }
                if (func->GetType() == ftLoop) {
                    cTemplateLoopFunction *funcsLoop =  dynamic_cast<cTemplateLoopFunction*>(func);
                    funcsLoop->InitIterator();
                    cTemplateFunction *loopFunc = NULL;
                    while(loopFunc = funcsLoop->GetNextFunction()) {
                        if (loopFunc->GetType() == ftDrawText || loopFunc->GetType() == ftDrawTextBox || func->GetType() == ftDrawTextVertical) {
                            string text = loopFunc->GetParameter(ptText);
                            string translation;
                            bool translated = globals->Translate(text, translation);
                            if (translated) {
                                loopFunc->SetTranslatedText(translation);
                            }
                        }
                    }
                }
            }
        }
    }

    //Translate Plugin Views
    for (map < string, map< int, cTemplateView*> >::iterator it = pluginViews.begin(); it != pluginViews.end(); it++) {
        map< int, cTemplateView*> plugViews = it->second;
        for (map< int, cTemplateView*>::iterator it2 = plugViews.begin(); it2 != plugViews.end(); it2++) {
            cTemplateView *pluginView = it2->second;
            pluginView->Translate();
        }
    }

    //Translate Subviews
    InitSubViewIterator();
    cTemplateView *subView = NULL;
    while(subView = GetNextSubView()) {
        subView->Translate();
    }
}

void cTemplateView::PreCache(bool isSubview) {

    if (!isSubview) {
        int osdW = cOsd::OsdWidth();
        int osdH = cOsd::OsdHeight();
        parameters->SetContainer(0, 0, osdW, osdH);
    } else {
        parameters->SetContainer(containerX, containerY, containerWidth, containerHeight);      
    }
    //Calculate OSD Size
    parameters->CalculateParameters();

    int osdX = parameters->GetNumericParameter(ptX);
    int osdY = parameters->GetNumericParameter(ptY);
    int osdWidth = parameters->GetNumericParameter(ptWidth);
    int osdHeight = parameters->GetNumericParameter(ptHeight);
    int pixOffset = 0;

    //Cache ViewElements
    for (map < eViewElement, cTemplateViewElement* >::iterator it = viewElements.begin(); it != viewElements.end(); it++) {
        cTemplateViewElement *viewElement = it->second;
        viewElement->SetGlobals(globals);
        if (!isSubview)
            viewElement->SetContainer(0, 0, osdWidth, osdHeight);
        else
            viewElement->SetContainer(osdX, osdY, osdWidth, osdHeight);
        viewElement->CalculateParameters();
        viewElement->CalculatePixmapParameters();
        viewElement->SetPixOffset(pixOffset);
        pixOffset += viewElement->GetNumPixmaps();
    }

    //Cache ViewGrids
    for (map < int, cTemplateViewGrid* >::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
        cTemplateViewGrid *viewGrid = it->second;
        viewGrid->SetGlobals(globals);
        viewGrid->SetContainer(0, 0, osdWidth, osdHeight);
        viewGrid->CalculateParameters();
        viewGrid->CalculatePixmapParameters();
    }

    //Cache ViewLists
    for (map < eViewList, cTemplateViewList* >::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        cTemplateViewList *viewList = it->second;
        viewList->SetGlobals(globals);
        //viewlists in subviews need complete container information
        if (isSubview)
            viewList->SetContainer(osdX, osdY, osdWidth, osdHeight);
        else
            viewList->SetContainer(0, 0, osdWidth, osdHeight);
        viewList->CalculateParameters();
        viewList->CalculateListParameters();
    }

    //Cache ViewTabs
    for (vector<cTemplateViewTab*>::iterator tab = viewTabs.begin(); tab != viewTabs.end(); tab++) {
        //viewtabs are only in subviews
        (*tab)->SetContainer(osdX, osdY, osdWidth, osdHeight);
        (*tab)->SetGlobals(globals);
        (*tab)->CalculateParameters();
    }

    //Cache Subviews
    for (map < eSubView, cTemplateView* >::iterator it = subViews.begin(); it != subViews.end(); it++) {
        cTemplateView *subView = it->second;
        subView->SetContainer(0, 0, osdWidth, osdHeight);
        subView->PreCache(true);
    }

    //Cache Plugin Subviews
    for (map < string, map< int, cTemplateView*> >::iterator it = pluginViews.begin(); it != pluginViews.end(); it++) {
        map< int, cTemplateView*> plugViews = it->second;
        for (map< int, cTemplateView*>::iterator it2 = plugViews.begin(); it2 != plugViews.end(); it2++) {
            cTemplateView *plugView = it2->second;
            plugView->SetContainer(0, 0, osdWidth, osdHeight);
            plugView->PreCache(true);
        }
    }
}

void cTemplateView::Debug(void) {

    esyslog("skindesigner: TemplateView %s", viewName.c_str());;
    
    parameters->Debug();

    for (map < eViewElement, cTemplateViewElement* >::iterator it = viewElements.begin(); it != viewElements.end(); it++) {
        esyslog("skindesigner: ++++++++ ViewElement: %s", GetViewElementName(it->first).c_str());
        cTemplateViewElement *viewElement = it->second;
        viewElement->Debug();
    }

    for (map < eViewList, cTemplateViewList* >::iterator it = viewLists.begin(); it != viewLists.end(); it++) {
        esyslog("skindesigner: ++++++++ ViewList: %s", GetViewListName(it->first).c_str());
        cTemplateViewList *viewList = it->second;
        viewList->Debug();
    }

    for (map < int, cTemplateViewGrid* >::iterator it = viewGrids.begin(); it != viewGrids.end(); it++) {
        esyslog("skindesigner: ++++++++ ViewGrid %d:", it->first);
        cTemplateViewGrid *viewGrid = it->second;
        viewGrid->Debug();
    }

    for (vector<cTemplateViewTab*>::iterator tab = viewTabs.begin(); tab != viewTabs.end(); tab++) {
        esyslog("skindesigner: ++++++++ ViewTab");
        (*tab)->Debug();
    }

    for (map < eSubView, cTemplateView* >::iterator it = subViews.begin(); it!= subViews.end(); it++) {
        esyslog("skindesigner: ++++++++ SubView: %s", GetSubViewName(it->first).c_str());
        cTemplateView *subView = it->second;
        subView->Debug();       
    }
    
    for (map < string, map< int, cTemplateView*> >::iterator it = pluginViews.begin(); it!= pluginViews.end(); it++) {
        esyslog("skindesigner: ++++++++ Plugin: %s", it->first.c_str());
        map< int, cTemplateView*> plugViews = it->second;
        for (map< int, cTemplateView*>::iterator it2 = plugViews.begin(); it2 != plugViews.end(); it2++) {
            esyslog("skindesigner: Tmpl %d", it2->first);
            ((cTemplateView*)it2->second)->Debug();
        }
    }

}


void cTemplateView::SetFunctionDefinitions(void) {
    
    string name = "viewelement";
    set<string> attributes;
    attributes.insert("debug");
    attributes.insert("detached");
    attributes.insert("delay");
    attributes.insert("fadetime");
    attributes.insert("name");
    attributes.insert("condition");
    attributes.insert("mode");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "listelement";
    attributes.clear();
    attributes.insert("debug");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "area";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("layer");
    attributes.insert("transparency");
    attributes.insert("background");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "areascroll";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("orientation");
    attributes.insert("delay");
    attributes.insert("mode");
    attributes.insert("scrollspeed");
    attributes.insert("condition");
    attributes.insert("scrollelement");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("layer");
    attributes.insert("transparency");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "loop";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("name");
    attributes.insert("orientation");
    attributes.insert("condition");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("columnwidth");
    attributes.insert("rowheight");
    attributes.insert("overflow");
    attributes.insert("maxitems");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "fill";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("color");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawtext";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("font");
    attributes.insert("fontsize");
    attributes.insert("color");
    attributes.insert("text");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawtextbox";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("align");
    attributes.insert("maxlines");  
    attributes.insert("font");
    attributes.insert("fontsize");
    attributes.insert("color");
    attributes.insert("text");
    attributes.insert("float");
    attributes.insert("floatwidth");
    attributes.insert("floatheight");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawtextvertical";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("height");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("direction");
    attributes.insert("font");
    attributes.insert("fontsize");
    attributes.insert("color");
    attributes.insert("text");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawimage";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("imagetype");
    attributes.insert("path");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("cache");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawrectangle";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("color");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawellipse";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("color");
    attributes.insert("quadrant");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));

    name = "drawslope";
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("condition");
    attributes.insert("name");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("align");
    attributes.insert("valign");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("color");
    attributes.insert("type");
    attributes.insert("animtype");
    attributes.insert("animfreq");
    funcsAllowed.insert(pair< string, set<string> >(name, attributes));
}

/************************************************************************************
* cTemplateViewChannel
************************************************************************************/

cTemplateViewChannel::cTemplateViewChannel(void) {

    viewName = "displaychannel";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    SetViewElements();
}

cTemplateViewChannel::~cTemplateViewChannel() {
}

void cTemplateViewChannel::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("channelinfo");
    viewElementsAllowed.insert("channelgroup");
    viewElementsAllowed.insert("epginfo");
    viewElementsAllowed.insert("progressbar");
    viewElementsAllowed.insert("progressbarback");
    viewElementsAllowed.insert("statusinfo");
    viewElementsAllowed.insert("audioinfo");
    viewElementsAllowed.insert("screenresolution");
    viewElementsAllowed.insert("signalquality");
    viewElementsAllowed.insert("signalqualityback");
    viewElementsAllowed.insert("devices");
    viewElementsAllowed.insert("currentweather");
    viewElementsAllowed.insert("scrapercontent");
    viewElementsAllowed.insert("datetime");
    viewElementsAllowed.insert("time");
    viewElementsAllowed.insert("message");
    viewElementsAllowed.insert("customtokens");
}

string cTemplateViewChannel::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veChannelInfo:
            name = "ChannelInfo";
            break;
        case veChannelGroup:
            name = "ChannelGroup";
            break;
        case veEpgInfo:
            name = "EpgInfo";
            break;
        case veProgressBar:
            name = "ProgressBar";
            break;
        case veProgressBarBack:
            name = "ProgressBar Background";
            break;
        case veStatusInfo:
            name = "StatusInfo";
            break;
        case veAudioInfo:
            name = "AudioInfo";
            break;
        case veScreenResolution:
            name = "Screen Resolution";
            break;
        case veSignalQuality:
            name = "Signal Quality";
            break;
        case veSignalQualityBack:
            name = "Signal Quality Background";
            break;
        case veDevices:
            name = "Devices";
            break;
        case veCurrentWeather:
            name = "Current Weather";
            break;
        case veScraperContent:
            name = "Scraper Content";
            break;
        case veDateTime:
            name = "DateTime";
            break;
        case veTime:
            name = "Time";
            break;
        case veMessage:
            name = "Message";
            break;
        case veCustomTokens:
            name = "Custom Tokens";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewChannel::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if (!sViewElement.compare("channelinfo")) {
        ve = veChannelInfo;
    } else if (!sViewElement.compare("channelgroup")) {
        ve = veChannelGroup;
    } else if (!sViewElement.compare("epginfo")) {
        ve = veEpgInfo;
    } else if (!sViewElement.compare("progressbar")) {
        ve = veProgressBar;
    } else if (!sViewElement.compare("progressbarback")) {
        ve = veProgressBarBack;
    } else if (!sViewElement.compare("statusinfo")) {
        ve = veStatusInfo;
    } else if (!sViewElement.compare("audioinfo")) {
        ve = veAudioInfo;
    } else if (!sViewElement.compare("screenresolution")) {
        ve = veScreenResolution;
    } else if (!sViewElement.compare("signalquality")) {
        ve = veSignalQuality;
    } else if (!sViewElement.compare("signalqualityback")) {
        ve = veSignalQualityBack;
    } else if (!sViewElement.compare("devices")) {
        ve = veDevices;
    } else if (!sViewElement.compare("currentweather")) {
        ve = veCurrentWeather;
    } else if (!sViewElement.compare("scrapercontent")) {
        ve = veScraperContent;
    } else if (!sViewElement.compare("datetime")) {
        ve = veDateTime;
    } else if (!sViewElement.compare("time")) {
        ve = veTime;
    } else if (!sViewElement.compare("message")) {
        ve = veMessage;
    } else if (!sViewElement.compare("customtokens")) {
        ve = veCustomTokens;
    }

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displaychannel: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

/************************************************************************************
* cTemplateViewMenu
************************************************************************************/

cTemplateViewMenu::cTemplateViewMenu(void) {

    viewName = "displaymenu";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    string subViewName = "menudefault";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menumain";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menusetup";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menuschedules";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menuchannels";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menutimers";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menurecordings";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menudetailedepg";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menudetailedrecording";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    subViewName = "menudetailedtext";
    //definition of allowed parameters for subtemplate menumain 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(subViewName, attributes));

    //definition of allowed parameters for menuitems viewlist 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("orientation");
    attributes.insert("align");
    attributes.insert("menuitemwidth");
    attributes.insert("determinatefont");
    attributes.insert("numlistelements");
    funcsAllowed.insert(pair< string, set<string> >("menuitems", attributes));

    //definition of allowed parameters for currentitems viewlist 
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("delay");
    attributes.insert("fadetime");
    funcsAllowed.insert(pair< string, set<string> >("currentelement", attributes));

    //definition of allowed parameters for viewtab
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("name");
    attributes.insert("condition");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("layer");
    attributes.insert("transparency");
    attributes.insert("scrollheight");
    funcsAllowed.insert(pair< string, set<string> >("tab", attributes));

    //definition of allowed parameters for plugin menus
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >("menuplugin", attributes));

    SetSubViews();
    SetViewElements();
    SetViewLists();

}

cTemplateViewMenu::~cTemplateViewMenu() {
}

void cTemplateViewMenu::SetSubViews(void) {
    subViewsAllowed.insert("menudefault");
    subViewsAllowed.insert("menumain");
    subViewsAllowed.insert("menusetup");
    subViewsAllowed.insert("menuschedules");
    subViewsAllowed.insert("menutimers");
    subViewsAllowed.insert("menurecordings");
    subViewsAllowed.insert("menuchannels");
    subViewsAllowed.insert("menudetailedepg");
    subViewsAllowed.insert("menudetailedrecording");
    subViewsAllowed.insert("menudetailedtext");
}

void cTemplateViewMenu::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("datetime");
    viewElementsAllowed.insert("time");
    viewElementsAllowed.insert("header");
    viewElementsAllowed.insert("colorbuttons");
    viewElementsAllowed.insert("message");
    viewElementsAllowed.insert("sortmode");
    viewElementsAllowed.insert("discusage");
    viewElementsAllowed.insert("systemload");
    viewElementsAllowed.insert("systemmemory");
    viewElementsAllowed.insert("temperatures");
    viewElementsAllowed.insert("timers");
    viewElementsAllowed.insert("lastrecordings");
    viewElementsAllowed.insert("devices");
    viewElementsAllowed.insert("currentweather");
    viewElementsAllowed.insert("currentschedule");
    viewElementsAllowed.insert("customtokens");
    viewElementsAllowed.insert("scrollbar");
    viewElementsAllowed.insert("detailheader");
    viewElementsAllowed.insert("tablabels");
}

void cTemplateViewMenu::SetViewLists(void) {
    viewListsAllowed.insert("menuitems");
}

string cTemplateViewMenu::GetSubViewName(eSubView sv) {
    string name;
    switch (sv) {
        case svMenuDefault:
            name = "Default Menu";
            break;
        case svMenuMain:
            name = "Main Menu";
            break;
        case svMenuSetup:
            name = "Setup Menu";
            break;
        case svMenuSchedules:
            name = "Schedules Menu";
            break;
        case svMenuTimers:
            name = "Timers Menu";
            break;
        case svMenuRecordings:
            name = "Recordings Menu";
            break;
        case svMenuChannels:
            name = "Channels Menu";
            break;
        case svMenuDetailedEpg:
            name = "Detailed EPG";
            break;
        case svMenuDetailedRecording:
            name = "Detailed Recording";
            break;
        case svMenuDetailedText:
            name = "Detailed Text";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}


string cTemplateViewMenu::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veDateTime:
            name = "DateTime";
            break;
        case veTime:
            name = "Time";
            break;
        case veHeader:
            name = "Header";
            break;
        case veButtons:
            name = "Color Buttons";
            break;
        case veMessage:
            name = "Message";
            break;
        case veSortMode:
            name = "Sort Mode";
            break;
        case veDiscUsage:
            name = "Disc Usage";
            break;
        case veSystemLoad:
            name = "System Load";
            break;
        case veSystemMemory:
            name = "System Memory";
            break;
        case veTemperatures:
            name = "Temperatures";
            break;
        case veTimers:
            name = "Timers";
            break;
        case veLastRecordings:
            name = "Last Recordings";
            break;
        case veCurrentSchedule:
            name = "Current Schedule";
            break;
        case veCurrentWeather:
            name = "Current Weather";
            break;
        case veCustomTokens:
            name = "Custom Tokens";
            break;
        case veDevices:
            name = "Devices";
            break;
        case veMenuItem:
            name = "Menu Item";
            break;
        case veMenuCurrentItemDetail:
            name = "Menu Current Item Detail";
            break;
        case veScrollbar:
            name = "Scrollbar";
            break;
        case veDetailHeader:
            name = "Detail header";
            break;
        case veTabLabels:
            name = "tab labels";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

string cTemplateViewMenu::GetViewListName(eViewList vl) {
    string name;
    switch (vl) {
        case vlMenuItem:
            name = "Menu Item";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewMenu::AddSubView(string sSubView, cTemplateView *subView) {
    eSubView sv = svUndefined;
    
    if (!sSubView.compare("menumain")) {
        sv = svMenuMain;
    } else if (!sSubView.compare("menudefault")) {
        sv = svMenuDefault;
    } else if (!sSubView.compare("menuschedules")) {
        sv = svMenuSchedules;
    } else if (!sSubView.compare("menusetup")) {
        sv = svMenuSetup;
    } else if (!sSubView.compare("menuschedules")) {
        sv = svMenuSchedules;
    } else if (!sSubView.compare("menutimers")) {
        sv = svMenuTimers;
    } else if (!sSubView.compare("menurecordings")) {
        sv = svMenuRecordings;
    } else if (!sSubView.compare("menuchannels")) {
        sv = svMenuChannels;
    } else if (!sSubView.compare("menudetailedepg")) {
        sv = svMenuDetailedEpg;
    } else if (!sSubView.compare("menudetailedrecording")) {
        sv = svMenuDetailedRecording;
    } else if (!sSubView.compare("menudetailedtext")) {
        sv = svMenuDetailedText;
    }

    if (sv == svUndefined) {
        esyslog("skindesigner: unknown SubView in displayMenu: %s", sSubView.c_str());
        return;
    }
    subView->SetGlobals(globals);
    subViews.insert(pair<eSubView, cTemplateView*>(sv, subView));
}

void cTemplateViewMenu::AddPluginView(string plugName, int templNo, cTemplateView *plugView) {
    plugView->SetGlobals(globals);

    map < string, map< int, cTemplateView*> >::iterator hit = pluginViews.find(plugName);

    if (hit == pluginViews.end()) {
        map< int, cTemplateView*> plugTemplates;
        plugTemplates.insert(pair<int, cTemplateView*>(templNo, plugView));
        pluginViews.insert(pair< string, map< int, cTemplateView*> >(plugName, plugTemplates));
    } else {
        hit->second.insert(pair<int, cTemplateView*>(templNo, plugView));
    }
}

void cTemplateViewMenu::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if (!sViewElement.compare("datetime")) {
        ve = veDateTime;
    } else if (!sViewElement.compare("time")) {
        ve = veTime;
    } else if (!sViewElement.compare("header")) {
        ve = veHeader;
    } else if (!sViewElement.compare("colorbuttons")) {
        ve = veButtons;
    } else if (!sViewElement.compare("message")) {
        ve = veMessage;
    } else if (!sViewElement.compare("sortmode")) {
        ve = veSortMode;
    } else if (!sViewElement.compare("discusage")) {
        ve = veDiscUsage;
    } else if (!sViewElement.compare("systemload")) {
        ve = veSystemLoad;
    } else if (!sViewElement.compare("systemmemory")) {
        ve = veSystemMemory;
    } else if (!sViewElement.compare("temperatures")) {
        ve = veTemperatures;
    } else if (!sViewElement.compare("timers")) {
        ve = veTimers;
    } else if (!sViewElement.compare("lastrecordings")) {
        ve = veLastRecordings;
    } else if (!sViewElement.compare("currentschedule")) {
        ve = veCurrentSchedule;
    } else if (!sViewElement.compare("customtokens")) {
        ve = veCustomTokens;
    } else if (!sViewElement.compare("devices")) {
        ve = veDevices;
    } else if (!sViewElement.compare("currentweather")) {
        ve = veCurrentWeather;
    } else if (!sViewElement.compare("scrollbar")) {
        ve = veScrollbar;
    } else if (!sViewElement.compare("detailheader")) {
        ve = veDetailHeader;
    } else if (!sViewElement.compare("tablabels")) {
        ve = veTabLabels;
    }

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displayMenu: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        if (viewElement->DebugTokens()) {
            dsyslog("skindesigner: activating token debugging for view element %s", sViewElement.c_str());
        }
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

void cTemplateViewMenu::AddViewList(string sViewList, cTemplateViewList *viewList) {
    
    eViewList vl = vlUndefined;
    if (!sViewList.compare("menuitems")) {
        vl = vlMenuItem;
    }

    if (vl == vlUndefined) {
        esyslog("skindesigner: unknown ViewList in displaymenu: %s", sViewList.c_str());
        return;
    }
    
    viewList->SetGlobals(globals);
    viewLists.insert(pair< eViewList, cTemplateViewList*>(vl, viewList));
}

void cTemplateViewMenu::AddViewTab(cTemplateViewTab *viewTab) {
    viewTabs.push_back(viewTab);
}

/************************************************************************************
* cTemplateViewMessage
************************************************************************************/

cTemplateViewMessage::cTemplateViewMessage(void) {

    viewName = "displaymessage";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    SetViewElements();
}

cTemplateViewMessage::~cTemplateViewMessage() {
}

void cTemplateViewMessage::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("message");
}

string cTemplateViewMessage::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veMessage:
            name = "Message";
            break;      
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewMessage::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if (!sViewElement.compare("message")) {
        ve = veMessage;
    } 

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displaymessage: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

/************************************************************************************
* cTemplateViewReplay
************************************************************************************/

cTemplateViewReplay::cTemplateViewReplay(void) {

    viewName = "displayreplay";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    //definition of allowed parameters for onpause and onpausemodeonly viewelement 
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("delay");
    attributes.insert("fadetime");
    funcsAllowed.insert(pair< string, set<string> >("onpause", attributes));
    funcsAllowed.insert(pair< string, set<string> >("onpausemodeonly", attributes));

    SetViewElements();
}

cTemplateViewReplay::~cTemplateViewReplay() {
}

void cTemplateViewReplay::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("backgroundmodeonly");
    viewElementsAllowed.insert("datetime");
    viewElementsAllowed.insert("time");
    viewElementsAllowed.insert("rectitle");
    viewElementsAllowed.insert("recinfo");
    viewElementsAllowed.insert("scrapercontent");
    viewElementsAllowed.insert("currenttime");
    viewElementsAllowed.insert("totaltime");
    viewElementsAllowed.insert("endtime");
    viewElementsAllowed.insert("progressbar");
    viewElementsAllowed.insert("cutmarks");
    viewElementsAllowed.insert("controlicons");
    viewElementsAllowed.insert("controliconsmodeonly");
    viewElementsAllowed.insert("jump");
    viewElementsAllowed.insert("message");
    viewElementsAllowed.insert("onpause");
    viewElementsAllowed.insert("onpausemodeonly");
    viewElementsAllowed.insert("customtokens");
}

string cTemplateViewReplay::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veDateTime:
            name = "DateTime";
            break;
        case veTime:
            name = "DateTime";
            break;
        case veRecTitle:
            name = "Recording Title";
            break;      
        case veRecInfo:
            name = "Recording Information";
            break;      
        case veRecCurrent:
            name = "Recording current Time";
            break;
        case veRecTotal:
            name = "Recording total Time";
            break;
        case veRecEnd:
            name = "Recording end Time";
            break;
        case veRecProgressBar:
            name = "Rec Progress Bar";
            break;
        case veCuttingMarks:
            name = "Cutting Marks";
            break;
        case veControlIcons:
            name = "Control Icons";
            break;
        case veControlIconsModeOnly:
            name = "Control Icons Mode only";
            break;
        case veBackgroundModeOnly:
            name = "Background Mode only";
            break;   
        case veRecJump:
            name = "Recording Jump";
            break;   
        case veScraperContent:
            name = "Scraper Content";
            break;
        case veOnPause:
            name = "On Pause";
            break;
        case veOnPauseModeOnly:
            name = "On Pause Mode Only";
            break;
        case veCustomTokens:
            name = "Custom Tokens";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewReplay::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if (!sViewElement.compare("datetime")) {
        ve = veDateTime;
    } else if (!sViewElement.compare("time")) {
        ve = veTime;
    } else if (!sViewElement.compare("rectitle")) {
        ve = veRecTitle;
    } else if (!sViewElement.compare("recinfo")) {
        ve = veRecInfo;
    } else if (!sViewElement.compare("scrapercontent")) {
        ve = veScraperContent;
    } else if (!sViewElement.compare("currenttime")) {
        ve = veRecCurrent;
    } else if (!sViewElement.compare("totaltime")) {
        ve = veRecTotal;
    } else if (!sViewElement.compare("endtime")) {
        ve = veRecEnd;
    } else if (!sViewElement.compare("progressbar")) {
        ve = veRecProgressBar;
    } else if (!sViewElement.compare("cutmarks")) {
        ve = veCuttingMarks;
    } else if (!sViewElement.compare("controlicons")) {
        ve = veControlIcons;
    } else if (!sViewElement.compare("controliconsmodeonly")) {
        ve = veControlIconsModeOnly;
    } else if (!sViewElement.compare("backgroundmodeonly")) {
        ve = veBackgroundModeOnly;
    } else if (!sViewElement.compare("jump")) {
        ve = veRecJump;
    } else if (!sViewElement.compare("message")) {
        ve = veMessage;
    } else if (!sViewElement.compare("onpause")) {
        ve = veOnPause;
    } else if (!sViewElement.compare("onpausemodeonly")) {
        ve = veOnPauseModeOnly;
    } else if (!sViewElement.compare("customtokens")) {
        ve = veCustomTokens;
    }

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displayreplay: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}


/************************************************************************************
* cTemplateViewVolume
************************************************************************************/

cTemplateViewVolume::cTemplateViewVolume(void) {

    viewName = "displayvolume";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    SetViewElements();
}

cTemplateViewVolume::~cTemplateViewVolume() {
}

void cTemplateViewVolume::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("volume");
}

string cTemplateViewVolume::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veVolume:
            name = "Volume";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewVolume::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if (!sViewElement.compare("volume")) {
        ve = veVolume;
    } 

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displayvolume: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

/************************************************************************************
* cTemplateViewAudioTracks
************************************************************************************/

cTemplateViewAudioTracks::cTemplateViewAudioTracks(void) {

    viewName = "displayaudiotracks";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    //definition of allowed parameters for menuitems viewlist 
    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("orientation");
    attributes.insert("align");
    attributes.insert("menuitemwidth");
    attributes.insert("numlistelements");
    funcsAllowed.insert(pair< string, set<string> >("menuitems", attributes));

    SetViewElements();
    SetViewLists();
}

cTemplateViewAudioTracks::~cTemplateViewAudioTracks() {
}

void cTemplateViewAudioTracks::SetViewElements(void) {
    viewElementsAllowed.insert("background");
    viewElementsAllowed.insert("header");
}

void cTemplateViewAudioTracks::SetViewLists(void) {
    viewListsAllowed.insert("menuitems");
}

string cTemplateViewAudioTracks::GetViewElementName(eViewElement ve) {
    string name;
    switch (ve) {
        case veBackground:
            name = "Background";
            break;
        case veHeader:
            name = "Header";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

string cTemplateViewAudioTracks::GetViewListName(eViewList vl) {
    string name;
    switch (vl) {
        case vlMenuItem:
            name = "Menu Item";
            break;
        default:
            name = "Unknown";
            break;
    };
    return name;
}

void cTemplateViewAudioTracks::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    
    if (!sViewElement.compare("background")) {
        ve = veBackground;
    } else if(!sViewElement.compare("header")) {
        ve = veHeader;
    }

    if (ve == veUndefined) {
        esyslog("skindesigner: unknown ViewElement in displayaudiotracks: %s", sViewElement.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

void cTemplateViewAudioTracks::AddViewList(string sViewList, cTemplateViewList *viewList) {
    
    eViewList vl = vlUndefined;
    if (!sViewList.compare("menuitems")) {
        vl = vlMenuItem;
    }

    if (vl == vlUndefined) {
        esyslog("skindesigner: unknown ViewList in displaymenu: %s", sViewList.c_str());
        return;
    }
    
    viewList->SetGlobals(globals);
    viewLists.insert(pair< eViewList, cTemplateViewList*>(vl, viewList));
}

/************************************************************************************
* cTemplateViewPlugin
************************************************************************************/

cTemplateViewPlugin::cTemplateViewPlugin(string pluginName, int viewID) {
    this->pluginName = pluginName;
    this->viewID = viewID;
    viewName = "displayplugin";
    //definition of allowed parameters for class itself 
    set<string> attributes;
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("fadetime");
    attributes.insert("scaletvx");
    attributes.insert("scaletvy");
    attributes.insert("scaletvwidth");
    attributes.insert("scaletvheight");
    attributes.insert("hideroot");
    attributes.insert("debuggrid");
    funcsAllowed.insert(pair< string, set<string> >(viewName, attributes));

    //definition of allowed parameters for viewtab
    attributes.clear();
    attributes.insert("debug");
    attributes.insert("name");
    attributes.insert("condition");
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("layer");
    attributes.insert("transparency");
    attributes.insert("scrollheight");
    funcsAllowed.insert(pair< string, set<string> >("tab", attributes));

    attributes.clear();
    attributes.insert("x");
    attributes.insert("y");
    attributes.insert("width");
    attributes.insert("height");
    attributes.insert("name");
    funcsAllowed.insert(pair< string, set<string> >("grid", attributes));

    viewElementsAllowed.insert("viewelement");
    viewElementsAllowed.insert("scrollbar");
    viewElementsAllowed.insert("tablabels");
    viewGridsAllowed.insert("grid");
}

cTemplateViewPlugin::~cTemplateViewPlugin() {
}

void cTemplateViewPlugin::AddSubView(string sSubView, cTemplateView *subView) {
    int subViewId = atoi(sSubView.c_str());
    subViews.insert(pair< eSubView, cTemplateView* >((eSubView)subViewId, subView));
}

void cTemplateViewPlugin::AddPixmap(string sViewElement, cTemplatePixmap *pix, vector<pair<string, string> > &viewElementattributes) {
    eViewElement ve = veUndefined;
    string viewElementName = "";
    int viewElementID = -1;
    bool found = false;
    for (vector<pair<string, string> >::iterator it = viewElementattributes.begin(); it != viewElementattributes.end(); it++) {
        if (!(it->first).compare("name")) {
            viewElementName = it->second;
            found = true;
            break;
        }
    }

    if (found) {
        viewElementID = config.GetPluginViewElementID(pluginName, viewElementName, viewID);
    } else {
        //check for internal view elements
        ePluginInteralViewElements pve = pveUndefined;
        if (!sViewElement.compare("scrollbar")) {
            pve = pveScrollbar;
        } else if (!sViewElement.compare("tablabels")) {
            pve = pveTablabels;
        }
        if (pve == pveUndefined) {
            esyslog("skindesigner: %s: unknown ViewElement in displayplugin: %s", pluginName.c_str(), viewElementName.c_str());
            return;
        }
        viewElementID = pve;
    }

    pix->SetGlobals(globals);

    ve = (eViewElement)viewElementID;
    map < eViewElement, cTemplateViewElement* >::iterator hit = viewElements.find(ve);
    if (hit == viewElements.end()) {
        cTemplateViewElement *viewElement = new cTemplateViewElement();
        viewElement->SetParameters(viewElementattributes);
        viewElement->AddPixmap(pix);
        viewElements.insert(pair< eViewElement, cTemplateViewElement*>(ve, viewElement));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

void cTemplateViewPlugin::AddPixmapGrid(cTemplatePixmap *pix, vector<pair<string, string> > &gridAttributes) {
    string gridName = "";
    bool found = false;
    for (vector<pair<string, string> >::iterator it = gridAttributes.begin(); it != gridAttributes.end(); it++) {
        if (!(it->first).compare("name")) {
            gridName = it->second;
            found = true;
            break;
        }
    }
    if (!found) {
        esyslog("skindesigner: no name defined for plugin %s grid", pluginName.c_str());
    }
    int gridID = config.GetPluginViewGridID(pluginName, gridName, viewID);

    if (gridID == -1) {
        esyslog("skindesigner: %s: unknown Grid in displayplugin: %s", pluginName.c_str(), gridName.c_str());
        return;
    }

    pix->SetGlobals(globals);

    map < int, cTemplateViewGrid* >::iterator hit = viewGrids.find(gridID);
    if (hit == viewGrids.end()) {
        cTemplateViewGrid *viewGrid = new cTemplateViewGrid();
        viewGrid->SetParameters(gridAttributes);
        viewGrid->AddPixmap(pix);
        viewGrids.insert(pair< int, cTemplateViewGrid*>(gridID, viewGrid));
    } else {
        (hit->second)->AddPixmap(pix);
    }
}

void cTemplateViewPlugin::AddViewTab(cTemplateViewTab *viewTab) {
    viewTabs.push_back(viewTab);
}
