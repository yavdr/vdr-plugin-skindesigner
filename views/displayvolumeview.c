#define __STL_CONFIG_H
#include <vdr/menu.h>
#include "displayvolumeview.h"

cDisplayVolumeView::cDisplayVolumeView(cTemplateView *tmplView) : cView(tmplView) {
    volumeLast = -1;
    muteLast = false;
    DeleteOsdOnExit();
    SetFadeTime(tmplView->GetNumericParameter(ptFadeTime));
}

cDisplayVolumeView::~cDisplayVolumeView() {
    CancelSave();
    FadeOut();
}

bool cDisplayVolumeView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    return ok;
}

void cDisplayVolumeView::DrawBackground(void) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    DrawViewElement(veBackground, &stringTokens, &intTokens);
}

void cDisplayVolumeView::DrawVolume(int current, int total, bool mute) {
    if ((volumeLast == current) && (muteLast == mute))
        return;
    volumeLast = current;
    muteLast = mute;
    
    map < string, string > stringTokens;
    map < string, int > intTokens;

    intTokens.insert(pair<string,int>("volume", current));
    intTokens.insert(pair<string,int>("maxvolume", total));
    intTokens.insert(pair<string,int>("volpercent", (double)current *100 / (double)total));
    intTokens.insert(pair<string,int>("mute", mute));

    ClearViewElement(veVolume);
    DrawViewElement(veVolume, &stringTokens, &intTokens);
}

void cDisplayVolumeView::Action(void) {
    SetInitFinished();
    FadeIn();
    DoFlush();
    cView::Action();
}
