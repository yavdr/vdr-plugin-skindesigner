#include <vdr/menu.h>
#include "displaymenulistview.h"


cDisplayMenuListView::cDisplayMenuListView(cTemplateViewList *tmplList, int count) {
    oneColumn = true;
    this->tmplList = tmplList;
    if (count < 0) {
        //if count is not set, the fixed number of items is configured in the template
        itemCount = tmplList->GetNumericParameter(ptNumElements);
    } else {
        //else use the actual numbers of elements
        itemCount = count;
        map < string, int > intTokens;
        intTokens.insert(pair<string,int>("numelements", count));
        tmplList->CalculateListParameters(&intTokens);
    }
    menuItems = new cDisplayMenuItemView*[itemCount];
    for (int i=0; i<itemCount; i++)
        menuItems[i] = NULL;
    tabs = new int[cSkinDisplayMenu::MaxTabs];
    for (int i=0; i<cSkinDisplayMenu::MaxTabs; i++)
        tabs[i] = 0;
    tabWidths = new int[cSkinDisplayMenu::MaxTabs];
    for (int i=0; i<cSkinDisplayMenu::MaxTabs; i++)
        tabWidths[i] = 0;
}

cDisplayMenuListView::~cDisplayMenuListView() {
    Clear();
    delete[] menuItems;
    delete[] tabs;
    delete[] tabWidths;
}

void cDisplayMenuListView::SetTabs(int tab1, int tab2, int tab3, int tab4, int tab5) {

    int menuItemWidth = 1920;
    int averageFontWidth = 20;
    if (tmplList) {
        menuItemWidth = tmplList->GetMenuItemWidth();
        averageFontWidth = tmplList->GetAverageFontWidth();
    }

    if (!tab1) {
        tabs[0] = 0;
        tabWidths[0] = menuItemWidth;
    } else {
        tabs[0] = 0;
        if (!tab2) {
            tabs[1] = menuItemWidth/2;
            tabWidths[0] = tabs[1];
            tabWidths[1] = tabs[1];        
        } else {
            tabs[0] = 0;
            tabs[1] = tab1 ? tabs[0] + tab1 : 0;
            tabs[2] = tab2 ? tabs[1] + tab2 : 0;
            tabs[3] = tab3 ? tabs[2] + tab3 : 0;
            tabs[4] = tab4 ? tabs[3] + tab4 : 0;
            tabs[5] = tab5 ? tabs[4] + tab5 : 0;
            for (int i = 1; i < cSkinDisplayMenu::MaxTabs; i++)
                tabs[i] *= averageFontWidth;

            for (int i = 0; i < cSkinDisplayMenu::MaxTabs; i++) {
                if (i == cSkinDisplayMenu::MaxTabs - 1) {
                    tabWidths[i] = menuItemWidth - tabs[i];
                } else {
                    tabWidths[i] = tabs[i+1] - tabs[i];
                }
            }
        }
    }
}

int cDisplayMenuListView::GetListWidth(void) {
    if (tmplList) {
        return tmplList->GetMenuItemWidth();
    }
    return 1920;
}


void cDisplayMenuListView::Clear(void) {
    for (int i=0; i<itemCount; i++) {
        if (menuItems[i]) {
            delete menuItems[i];
            menuItems[i] = NULL;
        }
    }
    oneColumn = true;
    for (int i=0; i<cSkinDisplayMenu::MaxTabs; i++) {
        tabs[i] = 0;
        tabWidths[i] = 0;
    }
}

void cDisplayMenuListView::AddDefaultMenuItem(int index, string *tabTexts, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        cDisplayMenuItemDefaultView *menuItem = dynamic_cast<cDisplayMenuItemDefaultView*>(menuItems[index]);
        if (!menuItem)
            return;
        menuItem->SetCurrent(current);
        menuItem->SetTabTexts(tabTexts);
        return;
    }
    for (int i=1; i<cSkinDisplayMenu::MaxTabs; i++) {
        if (tabTexts[i].size() > 0) {
            oneColumn = false;
            break;
        }
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemDefaultView(tmplList, tabTexts, tabs, tabWidths, current, selectable);
    menuItems[index] = item;
}

void cDisplayMenuListView::AddMainMenuItem(int index, const char *itemText, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemMainView(tmplList, itemText, current, selectable);
    menuItems[index] = item;
}

void cDisplayMenuListView::AddSetupMenuItem(int index, const char *itemText, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemMainView(tmplList, itemText, current, selectable);
    menuItems[index] = item;
}

void cDisplayMenuListView::AddSchedulesMenuItem(int index, const cEvent *event, const cChannel *channel, eTimerMatch timerMatch, 
                                                eMenuCategory cat, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemSchedulesView(tmplList, event, channel, timerMatch, cat, current, selectable);
    menuItems[index] = item;
}

void cDisplayMenuListView::AddChannelsMenuItem(int index, const cChannel *channel, bool withProvider, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemChannelsView(tmplList, channel, withProvider, current, selectable);
    menuItems[index] = item;    
}

void cDisplayMenuListView::AddTimersMenuItem(int index, const cTimer *timer, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemTimersView(tmplList, timer, current, selectable);
    menuItems[index] = item;        
}

void cDisplayMenuListView::AddRecordingMenuItem(int index, const cRecording *recording, int level, int total, int isNew, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemRecordingView(tmplList, recording, level, total, isNew, current, selectable);
    menuItems[index] = item;            
}

void cDisplayMenuListView::AddPluginMenuItem(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens, int index, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemPluginView(tmplList, stringTokens, intTokens, loopTokens, index, current, selectable);
    menuItems[index] = item;
}

void cDisplayMenuListView::AddTracksMenuItem(int index, const char *title, bool current, bool selectable) {
    if (index >= itemCount)
        return;
    if (menuItems[index]) {
        menuItems[index]->SetCurrent(current);
        return;
    }
    cDisplayMenuItemView *item = new cDisplayMenuItemTrackView(tmplList, title, current, selectable, itemCount);
    menuItems[index] = item;            
}

void cDisplayMenuListView::Render(void) {
    if (tabs[1] && oneColumn) {
        tabs[0] = 0;
        tabWidths[0] = tmplList->GetMenuItemWidth();
        for (int i=1; i<cSkinDisplayMenu::MaxTabs; i++) {
            tabs[i] = 0;
            tabWidths[i] = 0;
        }
    }
    int current = -1;
    for (int i=0; i<itemCount; i++) {
        if (menuItems[i] && menuItems[i]->Dirty()) {
            menuItems[i]->Clear();
            menuItems[i]->SetNumber(i);
            menuItems[i]->Prepare();
            menuItems[i]->SetTokens();
            menuItems[i]->Render();
            if (menuItems[i]->Current()) {
                current = i;
            } else {
                menuItems[i]->Stop();
            }
        }
    }
    if (current > -1) {
        menuItems[current]->Start();        
    }
}

void cDisplayMenuListView::Debug(void) {
    for (int i=0; i<itemCount; i++) {
        esyslog("skindesigner: item %d", i);
        if (menuItems[i]) {
            menuItems[i]->Debug();
        }
    }
}
