#ifndef __DISPLAYCHANNELVIEWELEMENTS_H
#define __DISPLAYCHANNELVIEWELEMENTS_H

#include "../libtemplate/template.h"
#include "view.h"
#include "viewhelpers.h"

class cViewElementDevices : public cViewElement, public cViewHelpers {
private:
    bool init;
    bool light;
public:
    cViewElementDevices(bool light, cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementDevices() {};
    bool Render(void);
};

enum ScraperContentType {
    ctPosterBanner,
    ctFull
};

class cViewElementScraperContent : public cViewElement, public cViewHelpers {
private:
    ScraperContentType type;
    const cEvent *event;
public:
    cViewElementScraperContent(const cEvent *event, ScraperContentType type, cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementScraperContent() {};
    void SetEvent(const cEvent *event) { this->event = event; };
    bool Render(void);
};

class cViewElementCustomTokens : public cViewElement, public cViewHelpers {
private:
    cTemplateView *tmplView;
public:
    cViewElementCustomTokens(cTemplateViewElement *tmplViewElement, cTemplateView *tmplView);
    virtual ~cViewElementCustomTokens() {};
    bool Render(void);
};

class cViewElementTimers : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementTimers(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementTimers() {};
    bool Render(void);
};

class cViewElementLastRecordings : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementLastRecordings(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementLastRecordings() {};
    bool Render(void);
};

class cViewElementCurrentSchedule : public cViewElement, public cViewHelpers {
private:
    string currentRecording;
public:
    cViewElementCurrentSchedule(cTemplateViewElement *tmplViewElement, string currentRecording);
    virtual ~cViewElementCurrentSchedule() {};
    bool Render(void);
};

class cViewElementMenuHeader : public cViewElement, public cViewHelpers {
private:
    eMenuCategory cat;
    string menuTitle;
public:
    cViewElementMenuHeader(cTemplateViewElement *tmplViewElement, eMenuCategory cat, string menuTitle);
    virtual ~cViewElementMenuHeader() {};
    bool Render(void);
};

class cViewElementMenuMessage : public cViewElement, public cViewHelpers {
private:
    eMessageType type;
    string text;
public:
    cViewElementMenuMessage(cTemplateViewElement *tmplViewElement, eMessageType type, string text);
    virtual ~cViewElementMenuMessage() {};
    bool Render(void);
};

#endif //__DISPLAYCHANNELVIEWELEMENTS_H