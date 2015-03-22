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
    void Render(void);
};

#endif //__DISPLAYCHANNELVIEWELEMENTS_H