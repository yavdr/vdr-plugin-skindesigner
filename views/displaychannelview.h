#ifndef __DISPLAYCHANNELVIEW_H
#define __DISPLAYCHANNELVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayChannelView : public cView {
private:
    cString lastDate;
    int lastScreenWidth;
    int lastScreenHeight;
    int lastSignalDisplay;
    int lastSignalStrength;
    int lastSignalQuality;
    string GetScreenResolutionString(int width, int height, bool *isHD);
    string GetScreenAspectString(double aspect, bool *isWideScreen);
    string GetChannelSep(const cChannel *channel, bool prev);
    virtual void Action(void);
public:
    cDisplayChannelView(cTemplateView *tmplView);
    virtual ~cDisplayChannelView();
    bool createOsd(void);
    void DrawBackground(void);
    void DrawChannel(cString &number, cString &name, cString &id, bool switching);
    void ClearChannel(void);
    void DrawDate(void);
    void DrawProgressBar(cString &start, cString &stop, int Current, int Total);
    void ClearProgressBar(void);
    void DrawProgressBarBack(void);
    void ClearProgressBarBack(void);
    void DrawEPGInfo(const cEvent *present, const cEvent *next, bool presentRecording, bool nextRecording);
    void ClearEPGInfo(void);
    void DrawStatusIcons(const cChannel *Channel);
    void ClearStatusIcons(void);
    void DrawScreenResolution(void);
    void ClearScreenResolution(void);
    void DrawScraperContent(const cEvent *event);
    void ClearScraperContent(void);
    void DrawSignal(void);
    void DrawSignalBackground(void);
    void ClearSignal(void);
    void ClearSignalBackground(void);
    void DrawChannelGroups(const cChannel *Channel, cString ChannelName);
    void ClearChannelGroups(void);
    void DisplayMessage(eMessageType Type, const char *Text);
    void DoStart(void) { Start(); };
    void Flush(void) { DoFlush(); };
};
#endif //__DISPLAYCHANNELVIEW_H
