#ifndef __DISPLAYREPLAYVIEW_H
#define __DISPLAYREPLAYVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayReplayView : public cView {
private:
    cString lastDate;
    string GetScreenResolutionString(int width, int height, bool *isHD);
    string GetScreenAspectString(double aspect, bool *isWideScreen);
    virtual void Action(void);
public:
    cDisplayReplayView(cTemplateView *tmplView);
    virtual ~cDisplayReplayView();
    bool createOsd(void);
    void DrawBackground(bool modeOnly);
    void DrawDate(bool modeOnly);
    void DrawTitle(const cRecording *recording);
    void DrawRecordingInformation(const cRecording *recording);
    void DrawScraperContent(const cRecording *recording);
    void DrawCurrent(const char *current);
    void DrawTotal(const char *total);
    void DrawProgressBar(int current, int total);
    void DrawMarks(const cMarks *marks, int total);
    void DrawControlIcons(bool play, bool forward, int speed, bool modeOnly);
    void DrawJump(const char *jump);
    void DrawMessage(eMessageType type, const char *text);
    void DoFadeIn(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYREPLAYVIEW_H
