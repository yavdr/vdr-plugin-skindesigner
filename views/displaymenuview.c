#define __STL_CONFIG_H
#include <vdr/menu.h>
#include <vdr/videodir.h>
#include "displaymenuview.h"
#include "displayviewelements.h"
#include "../config.h"
#include "../libcore/helpers.h"
#include "../services/scraper2vdr.h"

cDisplayMenuView::cDisplayMenuView(cTemplateView *tmplView, bool menuInit) : cView(tmplView) {
    if (!menuInit)
        SetFadeTime(0);
    cat = mcUndefined;
    sortMode = msmUnknown;
    sortModeLast = msmUnknown;
    buttonTexts = NULL;
}

cDisplayMenuView::~cDisplayMenuView() {
}

bool cDisplayMenuView::DrawBackground(void) {
    if (!ExecuteViewElement(veBackground)) {
        return false;
    }
    map < string, string > stringTokens;
    map < string, int > intTokens;

    DrawViewElement(veBackground, &stringTokens, &intTokens);
    return true;
}

bool cDisplayMenuView::DrawHeader(void) {
    if (!ExecuteViewElement(veHeader)) {
        return false;
    }
    if (DetachViewElement(veHeader)) {
        cViewElement *viewElement = GetViewElement(veHeader);
        if (!viewElement) {
            viewElement = new cViewElementMenuHeader(tmplView->GetViewElement(veHeader), cat, menuTitle);
            AddViewElement(veHeader, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        SetMenuHeader(cat, menuTitle, stringTokens, intTokens);
        ClearViewElement(veHeader);
        DrawViewElement(veHeader, &stringTokens, &intTokens);
    }
    return true;
}

bool cDisplayMenuView::DrawDateTime(bool forced, bool &implemented) {
    if (!ExecuteViewElement(veDateTime)) {
        return false;
    }
    implemented = true;
    if (DetachViewElement(veDateTime)) {
        cViewElement *viewElement = GetViewElement(veDateTime);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veDateTime), this);
            viewElement->SetCallback(veDateTime, &cDisplayMenuView::SetDate);
            AddViewElement(veDateTime, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                if (!viewElement->Render())
                    return false;
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        
        if (!SetDate(forced, stringTokens, intTokens)) {
            return false;
        }

        ClearViewElement(veDateTime);
        DrawViewElement(veDateTime, &stringTokens, &intTokens);
    }
    return true;
}

bool cDisplayMenuView::DrawTime(bool forced, bool &implemented) {
    if (!ExecuteViewElement(veTime)) {
        return false;
    }
    implemented = true;

    if (DetachViewElement(veTime)) {
        cViewElement *viewElement = GetViewElement(veTime);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veTime), this);
            viewElement->SetCallback(veTime, &cDisplayMenuView::SetTime);
            AddViewElement(veTime, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                if (!viewElement->Render())
                    return false;
        }
    } else {

        map < string, string > stringTokens;
        map < string, int > intTokens;

        if (!SetTime(forced, stringTokens, intTokens)) {
            return false;
        }    

        ClearViewElement(veTime);
        DrawViewElement(veTime, &stringTokens, &intTokens);
    }
    return true;
}

bool cDisplayMenuView::DrawColorButtons(void) {
    if (!ExecuteViewElement(veButtons)) {
        return false;
    }
    map < string, string > stringTokens;
    map < string, int > intTokens;

    stringTokens.insert(pair<string,string>("red", buttonTexts ? buttonTexts[0] : ""));
    stringTokens.insert(pair<string,string>("green", buttonTexts ? buttonTexts[1]: ""));
    stringTokens.insert(pair<string,string>("yellow", buttonTexts ? buttonTexts[2]: ""));
    stringTokens.insert(pair<string,string>("blue", buttonTexts ? buttonTexts[3] : ""));

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
    return true;
}

bool cDisplayMenuView::DrawMessage(eMessageType type, const char *text) {
    if (!ExecuteViewElement(veMessage)) {
        return false;
    }
    if (!text) {
        if (DetachViewElement(veMessage))
            DestroyDetachedViewElement(veMessage);
        else
            ClearViewElement(veMessage);
        return true;
    }

   if (DetachViewElement(veMessage)) {
        cViewElement *viewElement = GetViewElement(veMessage);
        if (!viewElement) {
            viewElement = new cViewElementMenuMessage(tmplView->GetViewElement(veMessage), type, text);
            AddViewElement(veMessage, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;

        SetMenuMessage(type, text, stringTokens, intTokens);
        ClearViewElement(veMessage);
        DrawViewElement(veMessage, &stringTokens, &intTokens);
    }
    return true;
}

void cDisplayMenuView::DrawScrollbar(int numMax, int numDisplayed, int offset) {
    if (!ExecuteViewElement(veScrollbar)) {
        return;
    }
    map < string, string > stringTokens;
    map < string, int > intTokens;
    if (numDisplayed < 1)
        return;
    
    int barHeight = 0;
    if (numDisplayed < numMax)
        barHeight = 1000;
    else
        barHeight = (double)numMax * 1000 / (double)numDisplayed;
    
    int barOffset = (double)offset * 1000 / (double)numDisplayed;

    intTokens.insert(pair<string, int>("height", barHeight));
    intTokens.insert(pair<string, int>("offset", barOffset));

    ClearViewElement(veScrollbar);
    DrawViewElement(veScrollbar, &stringTokens, &intTokens);
}

bool cDisplayMenuView::DrawSortMode(void) {
    if (!ExecuteViewElement(veSortMode)) {
        return false;
    }
    if (sortMode == msmUnknown) {
        if (sortModeLast != msmUnknown)
            ClearViewElement(veSortMode);
        sortModeLast = msmUnknown;
        return true;
    }
    if (sortMode == sortModeLast)
        return true;
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
    return true;
}

bool cDisplayMenuView::BackgroundImplemented(void) {
    if (!ExecuteViewElement(veBackground)) {
        return false;
    }
    return true;
}


void cDisplayMenuView::Action(void) {
    SetInitFinished();
    FadeIn();
    DoFlush();
}

/************************************************************************
* cDisplayMenuMainView
************************************************************************/

cDisplayMenuMainView::cDisplayMenuMainView(cTemplateView *tmplView, bool menuInit, string currentRecording) : cDisplayMenuView(tmplView, menuInit) {
    this->currentRecording = currentRecording;
    initial = true;
    InitDevices();
}

cDisplayMenuMainView::~cDisplayMenuMainView() {
    CancelSave();
    FadeOut();
}

void cDisplayMenuMainView::DrawStaticViewElements(void) {
    DrawTimers();
    DrawLastRecordings();
    DrawDiscUsage();
    DrawTemperatures();
    DrawCurrentSchedule();
    DrawCurrentWeather();
    DrawCustomTokens();
}

bool cDisplayMenuMainView::DrawDynamicViewElements(void) {
    bool loadChanged = DrawLoad();
    bool memChanged = DrawMemory();
    bool vdrChanged = DrawVdrStats();
    bool devicesChanged = DrawDevices();
    initial = false;
    return loadChanged || memChanged || vdrChanged || devicesChanged;

}

void cDisplayMenuMainView::DrawTimers(void) {
    if (!ExecuteViewElement(veTimers)) {
        return;
    }
    if (DetachViewElement(veTimers)) {
        cViewElement *viewElement = GetViewElement(veTimers);
        if (!viewElement) {
            viewElement = new cViewElementTimers(tmplView->GetViewElement(veTimers));
            AddViewElement(veTimers, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        map < string, vector< map< string, string > > > timerLoopTokens;
        vector< map< string, string > > timers;

        SetTimers(&intTokens, &stringTokens, &timers);

        timerLoopTokens.insert(pair< string, vector< map< string, string > > >("timers", timers));

        ClearViewElement(veTimers);
        DrawViewElement(veTimers, &stringTokens, &intTokens, &timerLoopTokens);
    }    
}

void cDisplayMenuMainView::DrawLastRecordings(void) {
    if (!ExecuteViewElement(veLastRecordings)) {
        return;
    }

    if (DetachViewElement(veLastRecordings)) {
        cViewElement *viewElement = GetViewElement(veLastRecordings);
        if (!viewElement) {
            viewElement = new cViewElementLastRecordings(tmplView->GetViewElement(veLastRecordings));
            AddViewElement(veLastRecordings, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        map < string, vector< map< string, string > > > recordingLoopTokens;
        vector< map< string, string > > lastRecordings;

        SetLastRecordings(&intTokens, &stringTokens, &lastRecordings);
        recordingLoopTokens.insert(pair< string, vector< map< string, string > > >("recordings", lastRecordings));

        ClearViewElement(veLastRecordings);
        DrawViewElement(veLastRecordings, &stringTokens, &intTokens, &recordingLoopTokens);
    }
}

void cDisplayMenuMainView::DrawDiscUsage(void) {
    if (!ExecuteViewElement(veDiscUsage)) {
        return;
    }
    if (DetachViewElement(veDiscUsage)) {
        cViewElement *viewElement = GetViewElement(veDiscUsage);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veDiscUsage), this);
            viewElement->SetCallback(veDiscUsage, &cDisplayMenuMainView::SetDiscUsage);
            AddViewElement(veDiscUsage, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        SetDiscUsage(false, stringTokens, intTokens);
        ClearViewElement(veDiscUsage);
        DrawViewElement(veDiscUsage, &stringTokens, &intTokens);
    }
}

bool cDisplayMenuMainView::DrawLoad(void) {
    if (!ExecuteViewElement(veSystemLoad)) {
        return false;
    }
    bool changed = false;
    if (DetachViewElement(veSystemLoad)) {
        cViewElement *viewElement = GetViewElement(veSystemLoad);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veSystemLoad), this);
            viewElement->SetCallback(veSystemLoad, &cDisplayMenuMainView::SetSystemLoad);
            AddViewElement(veSystemLoad, viewElement);
            viewElement->Start();
            changed = true;
        } else {
            if (!viewElement->Starting())
                changed = viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        changed = SetSystemLoad(false, stringTokens, intTokens);
        if (changed) {
            ClearViewElement(veSystemLoad);
            DrawViewElement(veSystemLoad, &stringTokens, &intTokens);
        }
    }
    return changed;
}

bool cDisplayMenuMainView::DrawMemory(void) {
    if (!ExecuteViewElement(veSystemMemory)) {
        return false;
    }
    bool changed = false;
    if (DetachViewElement(veSystemMemory)) {
        cViewElement *viewElement = GetViewElement(veSystemMemory);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veSystemMemory), this);
            viewElement->SetCallback(veSystemMemory, &cDisplayMenuMainView::SetSystemMemory);
            AddViewElement(veSystemMemory, viewElement);
            viewElement->Start();
            changed = true;
        } else {
            if (!viewElement->Starting())
                changed = viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        changed = SetSystemMemory(false, stringTokens, intTokens);
        if (changed) {
            ClearViewElement(veSystemMemory);
            DrawViewElement(veSystemMemory, &stringTokens, &intTokens);
        }
    }
    return changed;
}

bool cDisplayMenuMainView::DrawVdrStats(void) {
    if (!ExecuteViewElement(veVDRStats)) {
        return false;
    }
    bool changed = false;
    if (DetachViewElement(veVDRStats)) {
        cViewElement *viewElement = GetViewElement(veVDRStats);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veVDRStats), this);
            viewElement->SetCallback(veVDRStats, &cDisplayMenuMainView::SetVDRStats);
            AddViewElement(veVDRStats, viewElement);
            viewElement->Start();
            changed = true;
        } else {
            if (!viewElement->Starting())
                changed = viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        changed = SetVDRStats(false, stringTokens, intTokens);
        if (changed) {
            ClearViewElement(veVDRStats);
            DrawViewElement(veVDRStats, &stringTokens, &intTokens);
        }
    }
    return changed;
}

void cDisplayMenuMainView::DrawTemperatures(void) {
    if (!ExecuteViewElement(veTemperatures)) {
        return;
    }
    if (DetachViewElement(veTemperatures)) {
        cViewElement *viewElement = GetViewElement(veTemperatures);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veTemperatures), this);
            viewElement->SetCallback(veTemperatures, &cDisplayMenuMainView::SetSystemTemperatures);
            AddViewElement(veTemperatures, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        bool changed = SetSystemTemperatures(false, stringTokens, intTokens);
        if (changed) {
            ClearViewElement(veTemperatures);
            DrawViewElement(veTemperatures, &stringTokens, &intTokens);
        }
    }
}

bool cDisplayMenuMainView::DrawDevices(void) {
    if (!ExecuteViewElement(veDevices)) {
        return false;
    }
    string mode = tmplView->GetViewElementMode(veDevices);
    bool light = false;
    if (!mode.compare("light")) {
        light = true;
    } 
    bool changed = false;
    if (DetachViewElement(veDevices)) {
        cViewElement *viewElement = GetViewElement(veDevices);
        if (!viewElement) {
            viewElement = new cViewElementDevices(light, tmplView->GetViewElement(veDevices));
            AddViewElement(veDevices, viewElement);
            viewElement->Start();
            changed = true;
        } else {
            if (!viewElement->Starting())
                changed = viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        map < string, vector< map< string, string > > > deviceLoopTokens;
        vector< map< string, string > > devices;

        changed = SetDevices(initial, light, &intTokens, &devices);
        if (!changed)
            return false;

        deviceLoopTokens.insert(pair< string, vector< map< string, string > > >("devices", devices));
        ClearViewElement(veDevices);
        DrawViewElement(veDevices, &stringTokens, &intTokens, &deviceLoopTokens);        
    }
    return changed;
}

void cDisplayMenuMainView::DrawCurrentSchedule(void) {
    if (!ExecuteViewElement(veCurrentSchedule)) {
        return;
    }
    if (DetachViewElement(veCurrentSchedule)) {
        cViewElement *viewElement = GetViewElement(veCurrentSchedule);
        if (!viewElement) {
            viewElement = new cViewElementCurrentSchedule(tmplView->GetViewElement(veCurrentSchedule), currentRecording);
            AddViewElement(veCurrentSchedule, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        SetCurrentSchedule(currentRecording, stringTokens, intTokens);
        ClearViewElement(veCurrentSchedule);
        DrawViewElement(veCurrentSchedule, &stringTokens, &intTokens);
    }
}

void cDisplayMenuMainView::DrawCurrentWeather(void) {
    if (!ExecuteViewElement(veCurrentWeather)) {
        return;
    }
    if (DetachViewElement(veCurrentWeather)) {
        cViewElement *viewElement = GetViewElement(veCurrentWeather);
        if (!viewElement) {
            viewElement = new cViewElement(tmplView->GetViewElement(veCurrentWeather), this);
            viewElement->SetCallback(veCurrentWeather, &cDisplayMenuMainView::SetCurrentWeatherTokens);
            AddViewElement(veCurrentWeather, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;
        ClearViewElement(veCurrentWeather);
        if (!SetCurrentWeatherTokens(false, stringTokens, intTokens)){
            return;
        }
        DrawViewElement(veCurrentWeather, &stringTokens, &intTokens);
    }
}

void cDisplayMenuMainView::DrawCustomTokens(void) {
    if (!ExecuteViewElement(veCustomTokens)) {
        return;
    }
    if (!tmplView)
        return;

    if (DetachViewElement(veCustomTokens)) {
        cViewElement *viewElement = GetViewElement(veCustomTokens);
        if (!viewElement) {
            viewElement = new cViewElementCustomTokens(tmplView->GetViewElement(veCustomTokens), tmplView);
            AddViewElement(veCustomTokens, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting())
                viewElement->Render();
        }
    } else {
        map < string, string > stringTokens = tmplView->GetCustomStringTokens();
        map < string, int > intTokens = tmplView->GetCustomIntTokens();
        DrawViewElement(veCustomTokens, &stringTokens, &intTokens);
    }
}

/************************************************************************
* cDisplayMenuSchedulesView
************************************************************************/

cDisplayMenuSchedulesView::cDisplayMenuSchedulesView(cTemplateView *tmplView, eMenuCategory menuCat, bool menuInit) : cDisplayMenuView(tmplView, menuInit) {
    cat = menuCat;
    channel = NULL;
    isEpgSearchFavoritesMenu = false;
}

cDisplayMenuSchedulesView::~cDisplayMenuSchedulesView() {
    CancelSave();
    FadeOut();
}

bool cDisplayMenuSchedulesView::DrawHeader(void) {
    if (!ExecuteViewElement(veHeader)) {
        return false;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    intTokens.insert(pair<string,int>("whatson", (cat == mcSchedule)&&(!isEpgSearchFavoritesMenu) ? true: false));
    intTokens.insert(pair<string,int>("whatsonnow", (cat == mcScheduleNow) ? true: false));
    intTokens.insert(pair<string,int>("whatsonnext", (cat == mcScheduleNext) ? true: false));
    intTokens.insert(pair<string,int>("whatsonfavorites", isEpgSearchFavoritesMenu ? true: false));

    stringTokens.insert(pair<string,string>("title", menuTitle));
    stringTokens.insert(pair<string,string>("vdrversion", VDRVERSION));
    if (channel) {
        stringTokens.insert(pair<string,string>("channelnumber", *cString::sprintf("%d", channel->Number())));
        stringTokens.insert(pair<string,string>("channelname", channel->Name()));
        string channelID = *(channel->GetChannelID().ToString());
        stringTokens.insert(pair<string,string>("channelid", channelID));
        intTokens.insert(pair<string, int>("channellogoexists", imgCache->LogoExists(channelID)));
    } else {
        stringTokens.insert(pair<string,string>("channelnumber", ""));
        stringTokens.insert(pair<string,string>("channelname", ""));
        stringTokens.insert(pair<string,string>("channelid", ""));
        intTokens.insert(pair<string, int>("channellogoexists", 0));        
    }
    bool hasIcon = false;
    string icon = imgCache->GetIconName(menuTitle, cat);
    if (icon.size() > 0)
        hasIcon = true;

    stringTokens.insert(pair<string,string>("icon", icon));
    intTokens.insert(pair<string,int>("hasicon", hasIcon));
    ClearViewElement(veHeader);
    DrawViewElement(veHeader, &stringTokens, &intTokens);
    return true;
}