                                                                                                                                                                                                                                                                #include "template.h"
#include "xmlparser.h"
#include "../config.h"

// --- cTemplate -------------------------------------------------------------

cTemplate::cTemplate(eViewType viewType, string pluginName, int viewID) {
    globals = NULL;
    rootView = NULL;
    this->viewType = viewType;
    CreateView(pluginName, viewID);
}

cTemplate::~cTemplate() {
    if (rootView)
        delete rootView;

}

/*******************************************************************
* Public Functions
*******************************************************************/
bool cTemplate::ReadFromXML(string xmlfile) {
    string xmlFile;
    switch (viewType) {
        case vtDisplayChannel:
            xmlFile = "displaychannel.xml";
            break;
        case vtDisplayMenu:
            xmlFile = "displaymenu.xml";
            break;
        case vtDisplayMessage:
            xmlFile = "displaymessage.xml";
            break;
        case vtDisplayReplay:
            xmlFile = "displayreplay.xml";
            break;
        case vtDisplayVolume:
            xmlFile = "displayvolume.xml";
            break;
        case vtDisplayAudioTracks:
            xmlFile = "displayaudiotracks.xml";
            break;
        case vtDisplayPlugin:
            xmlFile = xmlfile;
            break;
        default:
            return false;
    }
    
    cXmlParser parser;
    if (!parser.ReadView(rootView, xmlFile)) {
        return false;
    }
    if (!parser.ParseView()) {
        return false;
    }
    
    //read additional plugin menu templates
    bool ok = true;
    if (viewType == vtDisplayMenu) {
        config.InitPluginMenuIterator();
        map <int,string> *plugTemplates = NULL;
        string plugName;
        while ( plugTemplates = config.GetPluginTemplates(plugName) ) {
            for (map <int,string>::iterator it = plugTemplates->begin(); it != plugTemplates->end(); it++) {
                int templateNumber = it->first;
                stringstream templateName;
                templateName << "plug-" << plugName << "-" << it->second.c_str();
                if (parser.ReadPluginView(plugName, templateNumber, templateName.str())) {
                    ok = parser.ParsePluginView(plugName, templateNumber);
                } else {
                    dsyslog("skindesigner: template %s for plugin %s not available", templateName.str().c_str(), plugName.c_str());
                }
            }
        }
    }
    return ok;
}

void cTemplate::SetGlobals(cGlobals *globals) { 
    this->globals = globals;
    rootView->SetGlobals(globals); 
}

void cTemplate::Translate(void) {
    rootView->Translate();
}


void cTemplate::PreCache(void) {
    rootView->PreCache(false);
}

vector< pair<string, int> > cTemplate::GetUsedFonts(void) {
    vector< pair<string, int> > usedFonts;

    GetUsedFonts(rootView, usedFonts);

    rootView->InitSubViewIterator();
    cTemplateView *subView = NULL;
    while(subView = rootView->GetNextSubView()) {
        GetUsedFonts(subView, usedFonts);
    }
    
    return usedFonts;
}


void cTemplate::CacheImages(void) {
    CacheImages(rootView);
    rootView->InitSubViewIterator();
    cTemplateView *subView = NULL;
    while(subView = rootView->GetNextSubView()) {
        CacheImages(subView);
    }
}

bool cTemplate::SetSubViews(string plugName, int viewID) {
    map <int,string> subViews = config.GetPluginSubViews(plugName, viewID);

    if (subViews.size() == 0) {
        return true;
    }

    for (map<int,string>::iterator it = subViews.begin(); it != subViews.end(); it ++) {
        int subViewID = it->first;
        stringstream templateName;
        templateName << "plug-" << plugName << "-" << it->second;
        string subViewTemplate = templateName.str();
        cTemplateView *plgTemplateView = new cTemplateViewPlugin(plugName, subViewID);
        plgTemplateView->SetGlobals(globals);
        cXmlParser parser;
        if (!parser.ReadView(plgTemplateView, subViewTemplate)) {
            esyslog("skindesigner: error reading plugin %s template", plugName.c_str());
            delete plgTemplateView;
            return false;
        }
        if (!parser.ParseView()) {
            esyslog("skindesigner: error reading plugin %s template", plugName.c_str());
            delete plgTemplateView;
            return false;
        }
        stringstream svid;
        svid << subViewID;
        rootView->AddSubView(svid.str(), plgTemplateView);
    }
    return true;
}


void cTemplate::Debug(void) {
    rootView->Debug();
}

/*******************************************************************
* Private Functions
*******************************************************************/

void cTemplate::CreateView(string pluginName, int viewID) {
    switch (viewType) {
        case vtDisplayChannel:
            rootView = new cTemplateViewChannel();
            break;
        case vtDisplayMenu:
            rootView = new cTemplateViewMenu();
            break;
        case vtDisplayReplay:
            rootView = new cTemplateViewReplay();
            break;
        case vtDisplayVolume:
            rootView = new cTemplateViewVolume();
            break;
        case vtDisplayAudioTracks:
            rootView = new cTemplateViewAudioTracks();
            break;
        case vtDisplayMessage:
            rootView = new cTemplateViewMessage();
            break;
        case vtDisplayPlugin:
            rootView = new cTemplateViewPlugin(pluginName, viewID);
            break;
        default:
            esyslog("skindesigner: unknown view %d", viewType);
    }
}

void cTemplate::GetUsedFonts(cTemplateView *view, vector< pair<string, int> > &usedFonts) {
    //used fonts in viewElements
    view->InitViewElementIterator();
    cTemplateViewElement *viewElement = NULL;
    while(viewElement = view->GetNextViewElement()) {
        viewElement->InitPixmapIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewElement->GetNextPixmap()) {
            pix->InitFunctionIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText) {
                    usedFonts.push_back(pair<string,int>(func->GetFontName(), func->GetNumericParameter(ptFontSize)));
                }
            }
        }
    }
    //used fonts in viewLists pixmaps
    view->InitViewListIterator();
    cTemplateViewList *viewList = NULL;
    while(viewList = view->GetNextViewList()) {
        viewList->InitPixmapIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewList->GetNextPixmap()) {
            pix->InitFunctionIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText) {
                    usedFonts.push_back(pair<string,int>(func->GetFontName(), func->GetNumericParameter(ptFontSize)));
                }
            }
        }
        cTemplateViewElement *listElement = viewList->GetListElement();
        listElement->InitPixmapIterator();
        while(pix = listElement->GetNextPixmap()) {
            pix->InitFunctionIterator();
            cTemplateFunction *func = NULL;
            while(func = pix->GetNextFunction()) {
                if (func->GetType() == ftDrawText) {
                    usedFonts.push_back(pair<string,int>(func->GetFontName(), func->GetNumericParameter(ptFontSize)));
                }
            }
        }
    }
    //used fonts in viewTabs
    view->InitViewTabIterator();
    cTemplateViewTab *viewTab = NULL;
    while(viewTab = view->GetNextViewTab()) {
        viewTab->InitFunctionIterator();
        cTemplateFunction *func = NULL;
        while(func = viewTab->GetNextFunction()) {
            if (func->GetType() == ftDrawText) {
                usedFonts.push_back(pair<string,int>(func->GetFontName(), func->GetNumericParameter(ptFontSize)));
            }
        }
    }
}

void cTemplate::CacheImages(cTemplateView *view) {
    //used images in viewElements
    view->InitViewElementIterator();
    cTemplateViewElement *viewElement = NULL;
    while(viewElement = view->GetNextViewElement()) {
        viewElement->InitPixmapIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewElement->GetNextPixmap()) {
            CachePixmapImages(pix);
        }
    }
    //used images in viewLists pixmaps
    view->InitViewListIterator();
    cTemplateViewList *viewList = NULL;
    while(viewList = view->GetNextViewList()) {
        viewList->InitPixmapIterator();
        cTemplatePixmap *pix = NULL;
        while(pix = viewList->GetNextPixmap()) {
            CachePixmapImages(pix);
        }
        cTemplateViewElement *listElement = viewList->GetListElement();
        listElement->InitPixmapIterator();
        while(pix = listElement->GetNextPixmap()) {
            CachePixmapImages(pix);
        }
        cTemplateViewElement *currentElement = viewList->GetListElementCurrent();
        if (!currentElement) {
            continue;
        }
        currentElement->InitPixmapIterator();
        while(pix = currentElement->GetNextPixmap()) {
            CachePixmapImages(pix);
        }
    }
    //used images in viewTabs
    view->InitViewTabIterator();
    cTemplateViewTab *viewTab = NULL;
    while(viewTab = view->GetNextViewTab()) {
        CachePixmapImages(viewTab);
    }    
}

void cTemplate::CachePixmapImages(cTemplatePixmap *pix) {
    pix->InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = pix->GetNextFunction()) {
        if (func->GetType() == ftDrawImage) {
            CacheImage(func);
        }
    }
}

void cTemplate::CacheImage(cTemplateFunction *func) {
    eImageType imgType = (eImageType)func->GetNumericParameter(ptImageType);
    int width = func->GetNumericParameter(ptWidth);
    int height = func->GetNumericParameter(ptHeight);

    switch (imgType) {
        case itIcon:
        case itMenuIcon: {
            string path = func->GetParameter(ptPath);
            imgCache->CacheIcon(imgType, path, width, height);
            break; }
        case itChannelLogo: {
            string doCache = func->GetParameter(ptCache);
            if (!doCache.compare("true")) {
                imgCache->CacheLogo(width, height);
            }
            break; }
        case itSkinPart: {
            string path = func->GetParameter(ptPath);
            imgCache->CacheSkinpart(path, width, height);
            break; }
        default:
            break;
    }
}
