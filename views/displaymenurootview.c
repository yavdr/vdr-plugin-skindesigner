#define __STL_CONFIG_H
#include <vdr/menu.h>
#include "displaymenurootview.h"
#include "displayviewelements.h"
#include "../config.h"
#include "../libcore/helpers.h"

cDisplayMenuRootView::cDisplayMenuRootView(cTemplateView *rootView) : cView(rootView) {
    cat = mcUndefined;
    sortMode = msmUnknown;
    sortModeLast = msmUnknown;
    menuTitle = "";
    viewType = svUndefined;
    subView = NULL;
    subViewAvailable = false;
    pluginName = "";
    pluginMenu = -1;
    pluginMenuType = mtUnknown;
    pluginMenuChanged = false;
    view = NULL;
    listView = NULL;
    detailView = NULL;
    buttonTexts[0] = "";
    buttonTexts[1] = "";
    buttonTexts[2] = "";
    buttonTexts[3] = "";
    defaultBackgroundDrawn = false;
    defaultHeaderDrawn = false;
    defaultButtonsDrawn = false;
    defaultDateTimeDrawn = false;
    defaultMessageDrawn = false;
    defaultSortmodeDrawn = false;
    DeleteOsdOnExit();
    SetFadeTime(tmplView->GetNumericParameter(ptFadeTime));
}

cDisplayMenuRootView::~cDisplayMenuRootView() {
    CancelSave();
    if (view) {
        delete view;
        view = NULL;
    }
    if (listView) {
        delete listView;
        listView = NULL;
    }
    if (detailView) {
        delete detailView;
        detailView = NULL;
    }
}

/*******************************************************************
* Public Functions
*******************************************************************/

bool cDisplayMenuRootView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    return ok;
}

/* Categories:
-1 mcUndefined,
0  mcUnknown,
1  mcMain,
2  mcSchedule,
3  mcScheduleNow,
4  mcScheduleNext,
5  mcChannel,
6  mcChannelEdit,
7  mcTimer,
8  mcTimerEdit,
9  mcRecording,
10 mcRecordingInfo,
11 mcRecordingEdit,
12 mcPlugin,
13 mcPluginSetup,
14 mcSetup,
15 mcSetupOsd,
16 mcSetupEpg,
17 mcSetupDvb,
18 mcSetupLnb,
19 mcSetupCam,
20 mcSetupRecord,
21 mcSetupReplay,
22 mcSetupMisc,
23 mcSetupPlugins,
24 mcCommand,
25 mcEvent,
26 mcText,
27 mcFolder,
28 mcCam
*/

void cDisplayMenuRootView::SetMenu(eMenuCategory menuCat, bool menuInit) {
    eSubView newViewType = svUndefined;
    cat = menuCat;
    bool isListView = true;
    if (menuCat != mcPlugin) {
        pluginName = "";
        pluginMenu = -1;
        pluginMenuType = mtUnknown;
        pluginMenuChanged = false;
    }
    switch (menuCat) {
        case mcMain:
            newViewType = svMenuMain;
            break;
       case mcSetup:
            newViewType = svMenuSetup;
            break;
        case mcSchedule:
        case mcScheduleNow:
        case mcScheduleNext:
            newViewType = svMenuSchedules;
            if (view)
                view->SetMenuCat(menuCat);
            break;
        case mcChannel:
            newViewType = svMenuChannels;
            break;
        case mcTimer:
            newViewType = svMenuTimers;
            break;
        case mcRecording:
            newViewType = svMenuRecordings;
            break;
        case mcEvent:
            newViewType = svMenuDetailedEpg;
            isListView = false;
            break;
        case mcRecordingInfo:
            newViewType = svMenuDetailedRecording;
            isListView = false;
            break;
        case mcText:
            newViewType = svMenuDetailedText;
            isListView = false;
            break;
        case mcPlugin:
            newViewType = svMenuPlugin;
            isListView = ( pluginMenuType == mtList ) ? true : false;
            break;
        default:
            newViewType = svMenuDefault;
            break;
    }
    if (newViewType != viewType || pluginMenuChanged) {
        if (newViewType == svMenuPlugin) {
            subView = tmplView->GetPluginView(pluginName, pluginMenu);
            if (!subView) {
                subViewAvailable = false;
                if (isListView)
                    subView = tmplView->GetSubView(svMenuDefault);
                else
                    subView = tmplView->GetSubView(svMenuDetailedText);
            } else {
                subViewAvailable = true;
            }
        } else {
            subView = tmplView->GetSubView(newViewType);
            if (!subView) {
                subViewAvailable = false;
                subView = tmplView->GetSubView(svMenuDefault);
            } else {
                subViewAvailable = true;
            }
        }
        //Cleanup
        if (view) {
            delete view;
            view = NULL;
        }
        if (listView) {
            delete listView;
            listView = NULL;
        }
        if (detailView) {
            delete detailView;
            detailView = NULL;
        }
        //Create new View
        switch (newViewType) {
            case svMenuMain:
                view = new cDisplayMenuMainView(subView, menuInit);
                break;
            case svMenuSchedules:
                if (subViewAvailable)
                    view = new cDisplayMenuSchedulesView(subView, menuCat, menuInit);
                else
                    view = new cDisplayMenuView(subView, menuInit);
                break;
            default:
                view = new cDisplayMenuView(subView, menuInit);
        }
        view->SetMenuCat(cat);
        view->SetButtonTexts(buttonTexts);
        //Cleanup root view
        ClearRootView();
        if (isListView) {
            //Create menu item list
            cTemplateViewList *tmplMenuItems = subView->GetViewList(vlMenuItem);
            if (!tmplMenuItems)
                return;
            listView = new cDisplayMenuListView(tmplMenuItems);
        } else {
            //Create detailed view
            detailView = new cDisplayMenuDetailView(subView);
        }
        viewType = newViewType;
    }
}

void cDisplayMenuRootView::SetSortMode(eMenuSortMode sortMode) {
    this->sortMode = sortMode;
    if (!view)
        return;
    view->SetSortMode(sortMode);
}

void cDisplayMenuRootView::CorrectDefaultMenu(void) {
    if (viewType > svMenuDefault && viewType != svMenuPlugin) {
        SetMenu(mcUnknown, true);
    }
}

void cDisplayMenuRootView::SetPluginMenu(string name, int menu, int type) {
    if (pluginName.compare(name) || menu != pluginMenu || type != pluginMenuType)
        pluginMenuChanged = true;
    else
        pluginMenuChanged = false;
    pluginName = name; 
    pluginMenu = menu; 
    pluginMenuType = (ePluginMenuType)type;
}

void cDisplayMenuRootView::SetTitle(const char *title) {
    menuTitle = title;
    if (view)
        view->SetTitle(title);
}

void cDisplayMenuRootView::SetButtonTexts(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (Red)
        buttonTexts[0] = Red;
    else
        buttonTexts[0] = "";
    if (Green)
        buttonTexts[1] = Green;
    else
        buttonTexts[1] = "";
    if (Yellow)
        buttonTexts[2] = Yellow;
    else
        buttonTexts[2] = "";
    if (Blue)
        buttonTexts[3] = Blue;
    else
        buttonTexts[3] = "";
    if (view)
        view->SetButtonTexts(buttonTexts);
}

void cDisplayMenuRootView::SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5) {
    if (listView) {
        listView->SetTabs(tab1, tab2, tab3, tab4, tab5);
    }  
}

void cDisplayMenuRootView::SetMessage(eMessageType type, const char *text) {
    if (!view)
        return;
    if (!view->DrawMessage(type, text)) {
        defaultMessageDrawn = true;
        DrawMessage(type, text);
    } else {
        defaultMessageDrawn = false;
    }
}

void cDisplayMenuRootView::SetDetailedViewEvent(const cEvent *event) {
    if (!detailView) {
        if (viewType != svMenuDetailedEpg) {
            SetMenu(mcEvent, true);
        } else {
            detailView = new cDisplayMenuDetailView(subView);
        }
    }
    detailView->SetEvent(event);
}

void cDisplayMenuRootView::SetDetailedViewRecording(const cRecording *recording) {
    if (!detailView) {
        if (viewType != svMenuDetailedRecording) {
            SetMenu(mcRecordingInfo, true);
        } else {
            detailView = new cDisplayMenuDetailView(subView);
        }
    }
    detailView->SetRecording(recording);
}

void cDisplayMenuRootView::SetDetailedViewText(const char *text) {
    if (!detailView) {
        if (viewType != svMenuDetailedText) {
            SetMenu(mcText, true);
        } else {
            detailView = new cDisplayMenuDetailView(subView);
        }
    }
    detailView->SetText(text);
}

bool cDisplayMenuRootView::SetDetailedViewPlugin(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens) {
    if (!detailView) {
        SetMenu(mcPlugin, true);
    }
    if (!subViewAvailable)
        return false;
    detailView->SetPluginTokens(stringTokens, intTokens, loopTokens);
    return true;
}

void cDisplayMenuRootView::KeyInput(bool up, bool page) {
    if (!detailView)
        return;

    if (up && page) {
        detailView->KeyLeft();
    } else if (!up && page) {
        detailView->KeyRight();
    } else if (up && !page) {
        detailView->KeyUp();
    } else if (!up && !page) {
        detailView->KeyDown();
    }
}

void cDisplayMenuRootView::Clear(void) {
    if (view) {
        view->ClearChannel();
        view->ClearEpgSearchFavorite();
    }
    if (listView) {
        listView->Clear();
    }
    if (detailView) {
        delete detailView;
        detailView = NULL;
    }
}

void cDisplayMenuRootView::ClearRootView(void) {
    if (defaultBackgroundDrawn && view && view->BackgroundImplemented())
        ClearViewElement(veBackground);
    if (defaultHeaderDrawn)
        ClearViewElement(veHeader);
    if (defaultButtonsDrawn)
        ClearViewElement(veButtons);
    if (defaultDateTimeDrawn)
        ClearViewElement(veDateTime);
    if (defaultMessageDrawn)
        ClearViewElement(veMessage);
    if (defaultSortmodeDrawn)
        ClearViewElement(veSortMode);
}

int cDisplayMenuRootView::GetMaxItems(void) {
    if (listView) {
        return listView->GetMaxItems();
    }
    //wrong menucat
    SetMenu(mcUnknown, true);
    return listView->GetMaxItems();
}

int cDisplayMenuRootView::GetListViewWidth(void) {
    if (listView) {
        return listView->GetListWidth();
    }
    return 0;    
}

int cDisplayMenuRootView::GetTextAreaWidth(void) {
    if (!tmplView)
        return 1900;
    cTemplateView *tmplSubView = tmplView->GetSubView(svMenuDefault);
    if (!tmplSubView)
        return 1900;
    int areaWidth = tmplSubView->GetNumericParameter(ptWidth);
    if (areaWidth > 0)
        return areaWidth;
    return 1900;
}

cFont *cDisplayMenuRootView::GetTextAreaFont(void) {
    if (!tmplView)
        return NULL;
    cTemplateView *tmplSubViewDefault = tmplView->GetSubView(svMenuDefault);
    if (!tmplSubViewDefault)
        return NULL;

    cTemplateViewList *tmplViewList = tmplSubViewDefault->GetViewList(vlMenuItem);
    if (!tmplViewList)
        return NULL;

    return tmplViewList->GetTextAreaFont();
}


void cDisplayMenuRootView::Render(void) {
    if (!view)
        return;
    if (!view->DrawBackground()) {
        defaultBackgroundDrawn = true;
        DrawBackground();
    } else {
        defaultBackgroundDrawn = false;
    }

    if (!view->DrawHeader()) {
        defaultHeaderDrawn = true;
        DrawHeader();
    } else {
        defaultHeaderDrawn = false;
    }

    if (!view->DrawSortMode()) {
        defaultSortmodeDrawn = true;
        DrawSortMode();
    } else {
        defaultSortmodeDrawn = false;
    }

    if (!view->DrawColorButtons()) {
        defaultButtonsDrawn = true;
        DrawColorButtons();
    } else {
        defaultButtonsDrawn = false;
    }

    view->DrawStaticViewElements();
    view->DrawDynamicViewElements();
}

void cDisplayMenuRootView::RenderMenuItems(void) {
    if (listView)
        listView->Render();
}

void cDisplayMenuRootView::RenderDetailView(void) {
    if (detailView)
        detailView->Render();
}

void cDisplayMenuRootView::RenderMenuScrollBar(int Total, int Offset) {
    if (!listView)
        return;
    view->DrawScrollbar(listView->GetMaxItems(), Total, Offset);
}

bool cDisplayMenuRootView::RenderDynamicElements(void) {
    if (!view)
        return false;
    bool updated = false;
    if (view->DrawTime()) {
        updated = true;
    } else if (DrawTime()) {
        updated = true;
    }
    if (view->DrawDynamicViewElements()){
        updated = true;
    }

    if (!view->DrawDateTime()) {
        defaultDateTimeDrawn = true;
        DrawDateTime();
    } else {
        defaultDateTimeDrawn = false;
    }

    return updated;
}

/*******************************************************************
* Private Functions
*******************************************************************/

void cDisplayMenuRootView::DrawBackground(void) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    DrawViewElement(veBackground, &stringTokens, &intTokens);
}

void cDisplayMenuRootView::DrawHeader(void) {
    if (!ExecuteViewElement(veHeader)) {
        return;
    }
    map < string, string > stringTokens;
    map < string, int > intTokens;
    SetMenuHeader(cat, menuTitle, stringTokens, intTokens);
    ClearViewElement(veHeader);
    DrawViewElement(veHeader, &stringTokens, &intTokens);
}

void cDisplayMenuRootView::DrawDateTime(void) {
    if (!ExecuteViewElement(veDateTime)) {
        return;
    }
    
    map < string, string > stringTokens;
    map < string, int > intTokens;
    
    if (!SetDate(stringTokens, intTokens)) {
        return;
    }

    ClearViewElement(veDateTime);
    DrawViewElement(veDateTime, &stringTokens, &intTokens);
}

bool cDisplayMenuRootView::DrawTime(void) {
    if (!ExecuteViewElement(veTime)) {
        return false;
    }
    
    map < string, string > stringTokens;
    map < string, int > intTokens;

    if (!SetTime(stringTokens, intTokens)) {
        return false;
    }

    ClearViewElement(veTime);
    DrawViewElement(veTime, &stringTokens, &intTokens);
    return true;
}

void cDisplayMenuRootView::DrawSortMode(void) {
    if (!ExecuteViewElement(veSortMode)) {
        return;
    }
    if (sortMode == msmUnknown) {
        if (sortModeLast != msmUnknown)
            ClearViewElement(veSortMode);
        sortModeLast = msmUnknown;
        return;
    }
    if (sortMode == sortModeLast) {
        return;
    }
    sortModeLast = sortMode;

    map < string, string > stringTokens;
    map < string, int > intTokens;

    bool sortNumber   = (sortMode == msmNumber)   ? true : false;
    bool sortName     = (sortMode == msmName)     ? true : false;
    bool sortTime     = (sortMode == msmTime)     ? true : false;
    bool sortProvider = (sortMode == msmProvider) ? true : false;

    intTokens.insert(pair<string, int>("sortnumber", sortNumber));
    intTokens.insert(pair<string, int>("sortname", sortName));
    intTokens.insert(pair<string, int>("sorttime", sortTime));
    intTokens.insert(pair<string, int>("sortprovider", sortProvider));

    ClearViewElement(veSortMode);
    DrawViewElement(veSortMode, &stringTokens, &intTokens);
    return;
}

void cDisplayMenuRootView::DrawColorButtons(void) {
    if (!ExecuteViewElement(veButtons)) {
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    stringTokens.insert(pair<string,string>("red", buttonTexts[0]));
    stringTokens.insert(pair<string,string>("green", buttonTexts[1]));
    stringTokens.insert(pair<string,string>("yellow", buttonTexts[2]));
    stringTokens.insert(pair<string,string>("blue", buttonTexts[3]));

    int colorKeys[4] = { Setup.ColorKey0, Setup.ColorKey1, Setup.ColorKey2, Setup.ColorKey3 };

    for (int button = 1; button < 5; button++) {
        string red = *cString::sprintf("red%d", button);
        string green = *cString::sprintf("green%d", button);
        string yellow = *cString::sprintf("yellow%d", button);
        string blue = *cString::sprintf("blue%d", button);
        bool isRed = false;
        bool isGreen = false;
        bool isYellow = false;
        bool isBlue = false;
        switch (colorKeys[button-1]) {
            case 0:
                isRed = true;
                break;
            case 1:
                isGreen = true;
                break;
            case 2:
                isYellow = true;
                break;
            case 3:
                isBlue = true;
                break;
            default:
                break;
        }
        intTokens.insert(pair<string, int>(red, isRed));
        intTokens.insert(pair<string, int>(green, isGreen));
        intTokens.insert(pair<string, int>(yellow, isYellow));
        intTokens.insert(pair<string, int>(blue, isBlue));
    }

    ClearViewElement(veButtons);
    DrawViewElement(veButtons, &stringTokens, &intTokens);
}

void cDisplayMenuRootView::DrawMessage(eMessageType type, const char *text) {
    if (!text) {
        ClearViewElement(veMessage);
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    intTokens.insert(pair<string, int>("status",  (type == mtStatus)  ? true : false));
    intTokens.insert(pair<string, int>("info",    (type == mtInfo)    ? true : false));
    intTokens.insert(pair<string, int>("warning", (type == mtWarning) ? true : false));
    intTokens.insert(pair<string, int>("error",   (type == mtError)   ? true : false));
    stringTokens.insert(pair<string,string>("text", text));

    ClearViewElement(veMessage);
    DrawViewElement(veMessage, &stringTokens, &intTokens);
}

void cDisplayMenuRootView::Action(void) {
    if (!view)
        return;
    SetInitFinished();
    Render();
    view->Start();
    FadeIn();
    DoFlush();
}
