#include "displayvolume.h"

#include "config.h"
#include "libcore/helpers.h"

cSDDisplayVolume::cSDDisplayVolume(cTemplate *volumeTemplate) {
    doOutput = true;
    initial = true;
    if (!volumeTemplate) {
        doOutput = false;
        esyslog("skindesigner: displayVolume no valid template - aborting");
        return;
    }
    volumeView = new cDisplayVolumeView(volumeTemplate->GetRootView());
    if (!volumeView->createOsd()) {
        doOutput = false;
    } else {
        volumeView->DrawBackground();
    }
}

cSDDisplayVolume::~cSDDisplayVolume() {
    if (!doOutput)
        return;
    delete volumeView;
}

void cSDDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    if (!doOutput)
        return;
    volumeView->DrawVolume(Current, Total, Mute);
}

void cSDDisplayVolume::Flush(void) {
    if (!doOutput)
        return;
    if (initial) {
        volumeView->DoFadeIn();
        initial = false;
    } else {
        volumeView->Flush();
    }
}
