#include "displayviewelements.h"

cViewElementDevices::cViewElementDevices(bool light, cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
    this->light = light;
    init = true;
}

bool cViewElementDevices::Render(void) {
    ClearTokens();
    map < string, vector<stringmap> > deviceLoopTokens;
    vector<stringmap> devices;

    if (init)
        InitDevices();
    bool changed = SetDevices(init, light, &intTokens, &devices);
    init = false;
    
    if (!changed)
        return false;

    deviceLoopTokens.insert(pair< string, vector< map< string, string > > >("devices", devices));
    
    ClearViewElement(veDevices);
    DrawViewElement(veDevices, &stringTokens, &intTokens, &deviceLoopTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementScraperContent::cViewElementScraperContent(const cEvent *event, ScraperContentType type, cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
    this->event = event;
    this->type = type;
}

bool cViewElementScraperContent::Render(void) {
    ClearTokens();
    SetPosterBanner(event, stringTokens, intTokens);
    ClearViewElement(veScraperContent);
    DrawViewElement(veScraperContent, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementCustomTokens::cViewElementCustomTokens(cTemplateViewElement *tmplViewElement, cTemplateView *tmplView) : cViewElement(tmplViewElement) {
    this->tmplView = tmplView;
}

bool cViewElementCustomTokens::Render(void) {
    ClearTokens();
    if (!tmplView)
        return false;
    stringTokens = tmplView->GetCustomStringTokens();
    intTokens = tmplView->GetCustomIntTokens();

    ClearViewElement(veCustomTokens);
    DrawViewElement(veCustomTokens, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementTimers::cViewElementTimers(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementTimers::Render(void) {
    ClearTokens();
    map < string, vector< map< string, string > > > timerLoopTokens;
    vector< map< string, string > > timers;

    SetTimers(&intTokens, &stringTokens, &timers);
    timerLoopTokens.insert(pair< string, vector< map< string, string > > >("timers", timers));

    ClearViewElement(veTimers);
    DrawViewElement(veTimers, &stringTokens, &intTokens, &timerLoopTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementLastRecordings::cViewElementLastRecordings(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementLastRecordings::Render(void) {
    ClearTokens();
    map < string, vector< map< string, string > > > recordingLoopTokens;
    vector< map< string, string > > lastRecordings;

    SetLastRecordings(&intTokens, &stringTokens, &lastRecordings);
    recordingLoopTokens.insert(pair< string, vector< map< string, string > > >("recordings", lastRecordings));

    ClearViewElement(veLastRecordings);
    DrawViewElement(veLastRecordings, &stringTokens, &intTokens, &recordingLoopTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementCurrentSchedule::cViewElementCurrentSchedule(cTemplateViewElement *tmplViewElement, string currentRecording) : cViewElement(tmplViewElement) {
    this->currentRecording = currentRecording;
}

bool cViewElementCurrentSchedule::Render(void) {
    ClearTokens();
    SetCurrentSchedule(currentRecording, stringTokens, intTokens);
    ClearViewElement(veCurrentSchedule);
    DrawViewElement(veCurrentSchedule, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementMenuHeader::cViewElementMenuHeader(cTemplateViewElement *tmplViewElement, eMenuCategory cat, string menuTitle) : cViewElement(tmplViewElement) {
    this->cat = cat;
    this->menuTitle = menuTitle;
}

bool cViewElementMenuHeader::Render(void) {
    ClearTokens();
    SetMenuHeader(cat, menuTitle, stringTokens, intTokens);
    ClearViewElement(veHeader);
    DrawViewElement(veHeader, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementMenuMessage::cViewElementMenuMessage(cTemplateViewElement *tmplViewElement, eMessageType type, string text) : cViewElement(tmplViewElement) {
    this->type = type;
    this->text = text;
}

bool cViewElementMenuMessage::Render(void) {
    ClearTokens();
    SetMenuMessage(type, text, stringTokens, intTokens);
    ClearViewElement(veMessage);
    DrawViewElement(veMessage, &stringTokens, &intTokens);
    return true;
}
