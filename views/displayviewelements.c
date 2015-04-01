#include "displayviewelements.h"

cViewElementDevices::cViewElementDevices(bool light, cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
    this->light = light;
    init = true;
}

bool cViewElementDevices::Render(void) {
    ClearTokens();
    map < string, vector< map< string, string > > > deviceLoopTokens;
    vector< map< string, string > > devices;

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

cViewElementSignal::cViewElementSignal(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementSignal::Render(void) {
    ClearTokens();
    bool changed = SetSignal(intTokens);
    if (!changed)
        return false;
    ClearViewElement(veSignalQuality);
    DrawViewElement(veSignalQuality, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementWeather::cViewElementWeather(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementWeather::Render(void) {
    ClearTokens();
    if (!SetCurrentWeatherTokens(stringTokens, intTokens)){
        ClearViewElement(veCurrentWeather);
        return false;
    }
    ClearViewElement(veCurrentWeather);
    DrawViewElement(veCurrentWeather, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementDate::cViewElementDate(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementDate::Render(void) {
    ClearTokens();
    if (!SetDate(stringTokens, intTokens)) {
        return false;
    }
    ClearViewElement(veDateTime);
    DrawViewElement(veDateTime, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementTime::cViewElementTime(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementTime::Render(void) {
    ClearTokens();
    if (!SetTime(stringTokens, intTokens)) {
        return false;
    }    
    ClearViewElement(veTime);
    DrawViewElement(veTime, &stringTokens, &intTokens);
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

cViewElementDiscUsage::cViewElementDiscUsage(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementDiscUsage::Render(void) {
    ClearTokens();
    SetDiscUsage(stringTokens, intTokens);

    ClearViewElement(veDiscUsage);
    DrawViewElement(veDiscUsage, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementSystemLoad::cViewElementSystemLoad(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementSystemLoad::Render(void) {
    ClearTokens();
    if (!SetSystemLoad(stringTokens, intTokens))
        return false;
    ClearViewElement(veSystemLoad);
    DrawViewElement(veSystemLoad, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementSystemMemory::cViewElementSystemMemory(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementSystemMemory::Render(void) {
    ClearTokens();
    if (!SetSystemMemory(stringTokens, intTokens))
        return false;
    ClearViewElement(veSystemMemory);
    DrawViewElement(veSystemMemory, &stringTokens, &intTokens);
    return true;
}

/********************************************************************************************************************/

cViewElementTemperature::cViewElementTemperature(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
}

bool cViewElementTemperature::Render(void) {
    ClearTokens();
    if (!SetSystemTemperatures(stringTokens, intTokens))
        return false;
    ClearViewElement(veTemperatures);
    DrawViewElement(veTemperatures, &stringTokens, &intTokens);
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
