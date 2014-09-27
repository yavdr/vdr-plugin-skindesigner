#include "displaytracks.h"


cSDDisplayTracks::cSDDisplayTracks(cTemplate *audiotracksTemplate, const char *Title, int NumTracks, const char * const *Tracks) {
    initial = true;
    numTracks = NumTracks;
    doOutput = true;
    currentTrack = 0;
    menuTitle = Title;
    if (!audiotracksTemplate) {
        esyslog("skindesigner: displayTracks no valid template - aborting");
        doOutput = false;
        return;
    }
    tracksView = new cDisplayAudiotracksView(NumTracks, audiotracksTemplate->GetRootView());
    if (!tracksView->createOsd()) {
        doOutput = false;
        return;
    }
    tracksView->DrawBackground();

    cDisplayMenuListView *list = tracksView->GetListView();
    if (list) {
        for (int i = 0; i < NumTracks; i++) {
            list->AddTracksMenuItem(i, Tracks[i], (i==currentTrack)?true:false, true);
        }
    }    
}

cSDDisplayTracks::~cSDDisplayTracks() {
    if (!doOutput)
        return;
    delete tracksView;
}

void cSDDisplayTracks::SetTrack(int Index, const char * const *Tracks) {
    cDisplayMenuListView *list = tracksView->GetListView();
    if (list) {
        list->AddTracksMenuItem(currentTrack, Tracks[currentTrack], false, true);
        list->AddTracksMenuItem(Index, Tracks[Index], true, true);
        currentTrack = Index;
    }
}

void cSDDisplayTracks::SetAudioChannel(int AudioChannel) {
    tracksView->DrawHeader(menuTitle, AudioChannel);
}

void cSDDisplayTracks::Flush(void) {
    if (!doOutput)
        return;
    if (initial) {
        tracksView->DoFadeIn();
    }
    initial = false;
    tracksView->RenderMenuItems();
    tracksView->Flush();
}
