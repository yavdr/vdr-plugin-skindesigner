#ifndef __DISPLAYREPLAYVIEW_H
#define __DISPLAYREPLAYVIEW_H

#include "../libtemplate/template.h"
#include "view.h"
#include "displayreplayonpauseview.h"

class cDisplayReplayView : public cView, public cViewHelpers {
private:
    int length;
    string endLast;
    cDisplayReplayOnPauseView *onPauseView;
    int numMarksLast;
    int *lastMarks;
    int markActive;
    bool MarksChanged(const cMarks *marks, int current);
    void RememberMarks(const cMarks *marks);
    virtual void Action(void);
public:
    cDisplayReplayView(cTemplateView *tmplView);
    virtual ~cDisplayReplayView();
    bool createOsd(void);
    void SetRecordingLength(int length) { this->length = length; };
    void DrawBackground(bool modeOnly);
    void DrawDate(void);
    void DrawTime(void);
    void DrawTitle(const cRecording *recording);
    void DrawTitle(const char *title);
    void DrawRecordingInformation(const cRecording *recording);
    void DrawScraperContent(const cRecording *recording);
    void DrawCurrent(const char *current);
    void DrawTotal(const char *total);
    void DrawEndTime(int current, int total);
    void DrawProgressBar(int current, int total);
    void DrawMarks(const cMarks *marks, int current, int total);
    void DrawControlIcons(bool play, bool forward, int speed, bool modeOnly);
    void DrawJump(const char *jump);
    void DrawMessage(eMessageType type, const char *text);
    void DrawOnPause(string recFileName, bool modeOnly);
    void ClearOnPause(void);
    void DelayOnPause(void);
    void DrawCustomTokens(void);
    void DoFadeIn(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYREPLAYVIEW_H
