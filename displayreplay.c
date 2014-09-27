#define __STL_CONFIG_H
#include "displayreplay.h"

cSDDisplayReplay::cSDDisplayReplay(cTemplate *replayTemplate, bool ModeOnly) {
    doOutput = true;
    initial = true;
    modeOnly = ModeOnly;
    numMarksLast = 0;
    lastMarks = NULL;
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
    replayView->DrawBackground(modeOnly);
}

cSDDisplayReplay::~cSDDisplayReplay() {
    if (!doOutput)
        return;
    delete replayView;
    if (lastMarks) {
        delete[] lastMarks;
    }
}

void cSDDisplayReplay::SetRecording(const cRecording *Recording) {
    if (!doOutput)
        return;
    replayView->DrawTitle(Recording);
    replayView->DrawRecordingInformation(Recording);
    replayView->DrawScraperContent(Recording);
}

void cSDDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    if (!doOutput)
        return;
    replayView->DrawControlIcons(Play, Forward, Speed, modeOnly);
}

void cSDDisplayReplay::SetProgress(int Current, int Total) {
    if (!doOutput)
        return;
    replayView->DrawProgressBar(Current, Total);
    if (MarksChanged()) {
        replayView->DrawMarks(marks, Total);
    }
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
    replayView->DrawDate(modeOnly);
    if (initial) {
        replayView->DoFadeIn();
        initial = false;
    } else {
        replayView->Flush();
    }
}

/****************************************************************************************
* Private Functions
*****************************************************************************************/

bool cSDDisplayReplay::MarksChanged(void) {
    if (!marks)
        return false;
    int numMarks = marks->Count();
    if (numMarks != numMarksLast) {
        RememberMarks();
        return true;
    }
    if (!lastMarks)
        return false;
    int i=0;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        if (m->Position() != lastMarks[i]) {
            RememberMarks();
            return true;
        }
        i++;
    }
    return false;
}

void cSDDisplayReplay::RememberMarks(void) {
    if (!marks)
        return;
    numMarksLast = marks->Count();
    if (numMarksLast < 1)
        return;
    if (lastMarks) {
        delete[] lastMarks;
    }
    lastMarks = new int[numMarksLast];
    int i=0;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        lastMarks[i] = m->Position();
        i++;
    }
}
