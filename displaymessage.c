#include "displaymessage.h"

cSDDisplayMessage::cSDDisplayMessage(cTemplate *messageTemplate) {
    messageView = NULL;
    doOutput = true;
    initial = true;
    if (!messageTemplate) {
        doOutput = false;
        esyslog("skindesigner: displayMessage no valid template - aborting");
        return;
    }
    messageView = new cDisplayMessageView(messageTemplate->GetRootView());
    if (!messageView->createOsd()) {
        doOutput = false;
        return;
    }
    messageView->DrawBackground();
}

cSDDisplayMessage::~cSDDisplayMessage() {
    if (messageView)
        delete messageView;
}

void cSDDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    if (!doOutput)
        return;
    messageView->ClearMessage();
    if (!Text) {
        return;
    }
    messageView->DrawMessage(Type, Text);
}


void cSDDisplayMessage::Flush(void) {
    if (!doOutput)
        return;
    if (initial) {
        messageView->DoFadeIn();
        initial = false;
    } else {
        messageView->Flush();
    }
}
