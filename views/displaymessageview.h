#ifndef __DISPLAYMESSAGEVIEW_H
#define __DISPLAYMESSAGEVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayMessageView : public cView {
private:
    virtual void Action(void);
public:
    cDisplayMessageView(cTemplateView *tmplView);
    virtual ~cDisplayMessageView();
    bool createOsd(void);
    void DrawBackground(void);
    void DrawMessage(eMessageType type, const char *text);
    void ClearMessage(void);
    void DoFadeIn(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYMESSAGEVIEW_H
