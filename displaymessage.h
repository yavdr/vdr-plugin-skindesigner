#ifndef __DISPLAYMESSAGE_H
#define __DISPLAYMESSAGE_H

#include <vdr/thread.h>
#include <vdr/skins.h>
#include "config.h"
#include "libtemplate/template.h"
#include "views/displaymessageview.h"

class cSDDisplayMessage : public cSkinDisplayMessage {
private:
    cDisplayMessageView *messageView;
    bool doOutput;
    bool initial;
public:
    cSDDisplayMessage(cTemplate *messageTemplate);
    virtual ~cSDDisplayMessage();
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};

#endif //__DISPLAYMESSAGE_H