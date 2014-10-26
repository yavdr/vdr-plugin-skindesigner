#define __STL_CONFIG_H
#include <vdr/menu.h>
#include <vdr/videodir.h>
#include "displaymenuview.h"
#include "../config.h"
#include "../libcore/helpers.h"
#include "../libcore/timers.h"

cDisplayMenuView::cDisplayMenuView(cTemplateView *tmplView, bool menuInit) : cView(tmplView) {
    if (menuInit)
        SetFadeTime(tmplView->GetNumericParameter(ptFadeTime));
    else
        SetFadeTime(0);
    cat = mcUndefined;
    buttonTexts = NULL;
}

cDisplayMenuView::~cDisplayMenuView() {
    CancelSave();
    FadeOut();
}

bool cDisplayMenuView::DrawBackground(void) {
    if (!ViewElementImplemented(veBackground)) {
        return false;
    }
    DrawViewElement(veBackground);
    return true;
}

bool cDisplayMenuView::DrawHeader(void) {
    if (!ViewElementImplemented(veHeader)) {
        return false;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    stringTokens.insert(pair<string,string>("title", menuTitle));
    stringTokens.insert(pair<string,string>("vdrversion", VDRVERSION));

    //check for standard menu entries
    bool hasIcon = false;
    string icon = imgCache->GetIconName(menuTitle, cat);
    if (icon.size() > 0)
        hasIcon = true;
    stringTokens.insert(pair<string,string>("icon", icon));
    intTokens.insert(pair<string,int>("hasicon", hasIcon));

    //Disc Usage
    string vdrUsageString = *cVideoDiskUsage::String();
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    string freeTime = *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60);
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;

    intTokens.insert(pair<string, int>("usedpercent", discUsage));
    intTokens.insert(pair<string, int>("freepercent", 100-discUsage));
    intTokens.insert(pair<string, int>("discalert", discAlert));
    intTokens.insert(pair<string, int>("freegb", freeGB));
    stringTokens.insert(pair<string,string>("freetime", freeTime));
    stringTokens.insert(pair<string,string>("vdrusagestring", vdrUsageString));

    ClearViewElement(veHeader);
    DrawViewElement(veHeader, &stringTokens, &intTokens);
    return true;
}

bool cDisplayMenuView::DrawDateTime(void) {
    if (!ViewElementImplemented(veDateTime)) {
        return false;
    }

    cString curDate = DayDateTime();

    map < string, string > stringTokens;
    map < string, int > intTokens;
    
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    
    intTokens.insert(pair<string, int>("year", now->tm_year + 1900));
    intTokens.insert(pair<string, int>("day", now->tm_mday));

    char monthname[20];
    char monthshort[10];
    strftime(monthshort, sizeof(monthshort), "%b", now);
    strftime(monthname, sizeof(monthname), "%B", now);

    stringTokens.insert(pair<string,string>("monthname", monthname));
    stringTokens.insert(pair<string,string>("monthnameshort", monthshort));
    stringTokens.insert(pair<string,string>("month", *cString::sprintf("%02d", now->tm_mon + 1)));
    stringTokens.insert(pair<string,string>("dayleadingzero", *cString::sprintf("%02d", now->tm_mday)));
    stringTokens.insert(pair<string,string>("dayname", *WeekDayNameFull(now->tm_wday)));
    stringTokens.insert(pair<string,string>("daynameshort", *WeekDayName(now->tm_wday)));
    stringTokens.insert(pair<string,string>("time", *TimeString(t)));

    ClearViewElement(veDateTime);
    DrawViewElement(veDateTime, &stringTokens, &intTokens);
    return true;
}

bool cDisplayMenuView::DrawColorButtons(void) {
    if (!ViewElementImplemented(veButtons)) {
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
    if (!ViewElementImplemented(veMessage)) {
        return false;
    }
    if (!text) {
        ClearViewElement(veMessage);
        return true;
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
    return true;
}

void cDisplayMenuView::DrawScrollbar(int numMax, int numDisplayed, int offset) {
    if (!ViewElementImplemented(veScrollbar)) {
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

bool cDisplayMenuView::BackgroundImplemented(void) {
    if (!ViewElementImplemented(veBackground)) {
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

cDisplayMenuMainView::cDisplayMenuMainView(cTemplateView *tmplView, bool menuInit) : cDisplayMenuView(tmplView, menuInit) {
    initial = true;
    lastSystemLoad = 0.0;
    InitDevices();
}

cDisplayMenuMainView::~cDisplayMenuMainView() {
    CancelSave();
    FadeOut();
}

void cDisplayMenuMainView::DrawStaticViewElements(void) {
    DrawTimers();
    DrawDiscUsage();
}

bool cDisplayMenuMainView::DrawDynamicViewElements(void) {
    bool loadChanged = DrawLoad();
    bool devicesChanged = DrawDevices();
    initial = false;
    return loadChanged || devicesChanged;

}

void cDisplayMenuMainView::DrawTimers(void) {
    if (!ViewElementImplemented(veTimers)) {
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    map < string, vector< map< string, string > > > timerLoopTokens;
    vector< map< string, string > > timers;

    cGlobalSortedTimers SortedTimers;// local and remote timers
    int numTimers = SortedTimers.Size();

    intTokens.insert(pair<string, int>("numtimers", numTimers));

    int numTimerConflicts = SortedTimers.NumTimerConfilicts();
    intTokens.insert(pair<string, int>("numtimerconflicts", numTimerConflicts));

    for (int i=0; i<15; i++) {
        stringstream name;
        name << "timer" << i+1 << "exists";
        if (i < numTimers) {
            intTokens.insert(pair<string, int>(name.str(), true));
        } else {
            intTokens.insert(pair<string, int>(name.str(), false));
        }
    }

    for (int i = 0; i < numTimers; i++) {
        if (i >=15)
            break;
        map< string, string > timerVals;
        const cTimer *Timer = SortedTimers[i];
        const cEvent *event = Timer->Event();
        if (event) {
            timerVals.insert(pair< string, string >("timers[title]", event->Title()));
        } else {
            const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(Timer->File(), FOLDERDELIMCHAR);
            if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
                File++;
            else
                File = Timer->File();
            timerVals.insert(pair< string, string >("timers[title]", File));            
        }
        const cChannel *channel = Timer->Channel();
        if (channel) {
            timerVals.insert(pair< string, string >("timers[channelname]", channel->Name()));
            stringstream chanNum;
            chanNum << channel->Number();
            timerVals.insert(pair< string, string >("timers[channelnumber]", chanNum.str()));
            string channelID = *(channel->GetChannelID().ToString());
            timerVals.insert(pair< string, string >("timers[channelid]", channelID));
            bool logoExists = imgCache->LogoExists(channelID);
            timerVals.insert(pair< string, string >("timers[channellogoexists]", logoExists ? "1" : "0"));
        } else {
            timerVals.insert(pair< string, string >("timers[channelname]", ""));
            timerVals.insert(pair< string, string >("timers[channelnumber]", "0"));
            timerVals.insert(pair< string, string >("timers[channelid]", ""));  
            timerVals.insert(pair< string, string >("timers[channellogoexists]", "0"));
        }
        
        timerVals.insert(pair< string, string >("timers[recording]", Timer->Recording() ? "1" : "0"));

        cString timerDate("");
        if (Timer->Recording()) {
            timerDate = cString::sprintf("-%s", *TimeString(Timer->StopTime()));
        } else {
            time_t Now = time(NULL);
            cString Today = WeekDayName(Now);
            cString Time = TimeString(Timer->StartTime());
            cString Day = WeekDayName(Timer->StartTime());
            if (Timer->StartTime() > Now + 6 * SECSINDAY) {
                time_t ttm = Timer->StartTime();
                struct tm * timerTime = localtime(&ttm);
                timerDate = cString::sprintf("%02d.%02d %s", timerTime->tm_mday, timerTime->tm_mon + 1, *Time);
            } else if (strcmp(Day, Today) != 0)
                timerDate = cString::sprintf("%s %s", *Day, *Time);
            else
                timerDate = Time;
            if (Timer->Flags() & tfVps)
                timerDate = cString::sprintf("VPS %s", *timerDate);
        }
        timerVals.insert(pair< string, string >("timers[datetime]", *timerDate));

        timers.push_back(timerVals);
    }

    timerLoopTokens.insert(pair< string, vector< map< string, string > > >("timers", timers));

    ClearViewElement(veTimers);
    DrawViewElement(veTimers, &stringTokens, &intTokens, &timerLoopTokens);
}

void cDisplayMenuMainView::DrawDiscUsage(void) {
    if (!ViewElementImplemented(veDiscUsage)) {
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    string vdrUsageString = *cVideoDiskUsage::String();
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    string freeTime = *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60);
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;

    intTokens.insert(pair<string, int>("usedpercent", discUsage));
    intTokens.insert(pair<string, int>("freepercent", 100-discUsage));
    intTokens.insert(pair<string, int>("discalert", discAlert));
    intTokens.insert(pair<string, int>("freegb", freeGB));
    stringTokens.insert(pair<string,string>("freetime", freeTime));
    stringTokens.insert(pair<string,string>("vdrusagestring", vdrUsageString));

    ClearViewElement(veDiscUsage);
    DrawViewElement(veDiscUsage, &stringTokens, &intTokens);
}

bool cDisplayMenuMainView::DrawLoad(void) {
    if (!ViewElementImplemented(veSystemLoad)) {
        return false;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    double systemLoad;
    if (getloadavg(&systemLoad, 1) > 0) {
        if (lastSystemLoad == systemLoad) {
            return false;
        }
        string load = *cString::sprintf("%.2f", systemLoad);
        stringTokens.insert(pair<string,string>("load", load));
        lastSystemLoad = systemLoad;
    }

    ClearViewElement(veSystemLoad);
    DrawViewElement(veSystemLoad, &stringTokens, &intTokens);

    return true;
}

bool cDisplayMenuMainView::DrawDevices(void) {
    if (!ViewElementImplemented(veDevices)) {
        return false;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;
    map < string, vector< map< string, string > > > deviceLoopTokens;
    vector< map< string, string > > devices;

    bool changed = SetDevices(initial, &intTokens, &devices);
    if (!changed)
        return false;

    deviceLoopTokens.insert(pair< string, vector< map< string, string > > >("devices", devices));
    
    ClearViewElement(veDevices);
    DrawViewElement(veDevices, &stringTokens, &intTokens, &deviceLoopTokens);
    return true;
}
/************************************************************************
* cDisplayMenuSchedulesView
************************************************************************/

cDisplayMenuSchedulesView::cDisplayMenuSchedulesView(cTemplateView *tmplView, eMenuCategory menuCat, bool menuInit) : cDisplayMenuView(tmplView, menuInit) {
    cat = menuCat;
    channel = NULL;
}

cDisplayMenuSchedulesView::~cDisplayMenuSchedulesView() {
    CancelSave();
    FadeOut();
}

bool cDisplayMenuSchedulesView::DrawHeader(void) {
    if (!ViewElementImplemented(veHeader)) {
        return false;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    intTokens.insert(pair<string,int>("whatson", (cat == mcSchedule) ? true: false));
    intTokens.insert(pair<string,int>("whatsonnow", (cat == mcScheduleNow) ? true: false));
    intTokens.insert(pair<string,int>("whatsonnext", (cat == mcScheduleNext) ? true: false));

    stringTokens.insert(pair<string,string>("title", menuTitle));
    stringTokens.insert(pair<string,string>("vdrversion", VDRVERSION));
    if (channel) {
        stringTokens.insert(pair<string,string>("channelnumber", *cString::sprintf("%d", channel->Number())));
        stringTokens.insert(pair<string,string>("channelname", channel->Name()));
        stringTokens.insert(pair<string,string>("channelid", *(channel->GetChannelID().ToString())));
        
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