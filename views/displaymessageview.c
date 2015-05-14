#define __STL_CONFIG_H
#include <vdr/menu.h>
#include "displaymessageview.h"

cDisplayMessageView::cDisplayMessageView(cTemplateView *tmplView) : cView(tmplView) {
    DeleteOsdOnExit();
}

cDisplayMessageView::~cDisplayMessageView() {
}

bool cDisplayMessageView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    if (!ok) {
        DeleteOsdOnExit(false);
    }
    return ok;
}

void cDisplayMessageView::DrawBackground(void) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    DrawViewElement(veBackground, &stringTokens, &intTokens);
}

void cDisplayMessageView::DrawMessage(eMessageType type, const char *text) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    stringTokens.insert(pair<string,string>("text", text));

    intTokens.insert(pair<string,int>("status",  (type == mtStatus)  ? true : false));
    intTokens.insert(pair<string,int>("info",    (type == mtInfo)    ? true : false));
    intTokens.insert(pair<string,int>("warning", (type == mtWarning) ? true : false));
    intTokens.insert(pair<string,int>("error",   (type == mtError)   ? true : false));

    DrawViewElement(veMessage, &stringTokens, &intTokens);
}

void cDisplayMessageView::ClearMessage(void) {
    ClearViewElement(veMessage);
}
