#ifndef __DISPLAYCHANNELVIEWELEMENTS_H
#define __DISPLAYCHANNELVIEWELEMENTS_H

#include "../libtemplate/template.h"
#include "view.h"
#include "viewhelpers.h"

class cViewElementDevices : public cViewElement, public cViewHelpers {
private:
    bool init;
public:
    cViewElementDevices(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementDevices() {};
    bool Render(void);
};

class cViewElementSignal : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementSignal(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementSignal() {};
    bool Render(void);
};

class cViewElementWeather : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementWeather(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementWeather() {};
    bool Render(void);
};

class cViewElementDate : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementDate(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementDate() {};
    bool Render(void);
};

class cViewElementTime : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementTime(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementTime() {};
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

class cViewElementDiscUsage : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementDiscUsage(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementDiscUsage() {};
    bool Render(void);
};

class cViewElementSystemLoad : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementSystemLoad(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementSystemLoad() {};
    bool Render(void);
};

class cViewElementSystemMemory : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementSystemMemory(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementSystemMemory() {};
    bool Render(void);
};

class cViewElementTemperature : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementTemperature(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementTemperature() {};
    bool Render(void);
};

class cViewElementCurrentSchedule : public cViewElement, public cViewHelpers {
private:
public:
    cViewElementCurrentSchedule(cTemplateViewElement *tmplViewElement);
    virtual ~cViewElementCurrentSchedule() {};
    bool Render(void);
};

#endif //__DISPLAYCHANNELVIEWELEMENTS_H