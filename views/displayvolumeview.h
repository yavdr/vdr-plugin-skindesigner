#ifndef __DISPLAYVOLUMEVIEW_H
#define __DISPLAYVOLUMEVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayVolumeView : public cView {
private:
    int volumeLast;
    bool muteLast;
    virtual void Action(void);
public:
    cDisplayVolumeView(cTemplateView *tmplView);
    virtual ~cDisplayVolumeView();
    bool createOsd(void);
    void DrawBackground(void);
    void DrawVolume(int current, int total, bool mute);
    void DoFadeIn(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYVOLUMEVIEW_H
