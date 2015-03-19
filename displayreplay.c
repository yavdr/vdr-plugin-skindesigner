#define __STL_CONFIG_H
#include <vdr/player.h>
#include "displayreplay.h"

cSDDisplayReplay::cSDDisplayReplay(cTemplate *replayTemplate, bool ModeOnly) {
    doOutput = true;
    initial = true;
    modeOnly = ModeOnly;
    replayView = NULL;
    if (!replayTemplate) {
        doOutput = false;
        esyslog("skindesigner: displayReplay no valid template - aborting");
        return;
    }
    replayView = new cDisplayReplayView(replayTemplate->GetRootView());
    if (!replayView->createOsd()) {
        doOutput = false;
        return;
    }
}

cSDDisplayReplay::~cSDDisplayReplay() {
    if (replayView)
        delete replayView;
}

void cSDDisplayReplay::SetRecording(const cRecording *Recording) {
    if (!doOutput || !Recording)
        return;
    replayView->DrawTitle(Recording);
    replayView->DrawRecordingInformation(Recording);
    replayView->DrawScraperContent(Recording);
}

void cSDDisplayReplay::SetTitle(const char *Title) {
    if (!doOutput || !Title)
        return;
    replayView->DrawTitle(Title);
}

void cSDDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    if (!doOutput)
        return;
    if (!Play && Speed < 0) {
        string recFileName = "";
        cControl *control = cControl::Control();
        if (control) {
            const cRecording *recording = control->GetRecording();
            if (recording && recording->FileName())
                recFileName = recording->FileName();
        }
        replayView->DrawOnPause(recFileName, modeOnly);
    } else {
        replayView->ClearOnPause();
    }
    replayView->DrawControlIcons(Play, Forward, Speed, modeOnly);
}

void cSDDisplayReplay::SetProgress(int Current, int Total) {
    if (!doOutput)
        return;
    replayView->DelayOnPause();
    replayView->DrawProgressBar(Current, Total);
    replayView->DrawMarks(marks, Current, Total);
}

void cSDDisplayReplay::SetCurrent(const char *Current) {
    if (!doOutput)
        return;
    replayView->DrawCurrent(Current);
}

void cSDDisplayReplay::SetTotal(const char *Total) {
    if (!doOutput)
        return;
    replayView->DrawTotal(Total);
}

void cSDDisplayReplay::SetJump(const char *Jump) {
    if (!doOutput)
        return;
    replayView->DrawJump(Jump);    
}

void cSDDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
    if (!doOutput)
        return;
    replayView->DrawMessage(Type, Text);
}

void cSDDisplayReplay::Flush(void) {
    if (!doOutput)
        return;
    if (!modeOnly) {
        replayView->DrawDate();
        replayView->DrawTime();
    }

    if (initial) {
        replayView->DrawBackground(modeOnly);
        replayView->DrawCustomTokens();
        replayView->DoFadeIn();
        initial = false;
    } else {
        replayView->Flush();
    }
}
