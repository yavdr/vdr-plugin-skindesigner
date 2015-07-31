#define __STL_CONFIG_H
#include <vdr/menu.h>
#include "../services/scraper2vdr.h"
#include "displayviewelements.h"
#include "displayreplayview.h"
#include "../libcore/helpers.h"

cDisplayReplayView::cDisplayReplayView(cTemplateView *tmplView) : cView(tmplView) {
    length = 0;
    timeShiftActive = false;
    timeShiftFramesTotal = 0;
    timeShiftLength = 1;
    timeShiftDuration = "";
    endLast = "";
    onPauseView = NULL;
    numMarksLast = 0;
    lastMarks = NULL;
    markActive = -1;
    DeleteOsdOnExit();
}

cDisplayReplayView::~cDisplayReplayView() {
    if (lastMarks) {
        delete[] lastMarks;
    }
    if (onPauseView) {
        delete onPauseView;
    }
}

bool cDisplayReplayView::createOsd(void) {
    cRect osdSize = tmplView->GetOsdSize();
    bool ok = CreateOsd(cOsd::OsdLeft() + osdSize.X(),
                        cOsd::OsdTop() + osdSize.Y(),
                        osdSize.Width(),
                        osdSize.Height());
    return ok;
}

void cDisplayReplayView::SetTimeShift(int framesTotal, int timeShiftLength) {
    timeShiftActive = true;
    timeShiftFramesTotal = framesTotal;
    this->timeShiftLength = timeShiftLength;
    int mins = (timeShiftLength / 60) % 60;
    int hours = (timeShiftLength / 3600) % 24;
    timeShiftDuration = *cString::sprintf("%d:%02d", hours, mins);
}

void cDisplayReplayView::DrawBackground(bool modeOnly) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    if (modeOnly)
        DrawViewElement(veBackgroundModeOnly, &stringTokens, &intTokens);
    else
        DrawViewElement(veBackground, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawDate(void) {
    if (!ExecuteViewElement(veDateTime)) {
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;
    
    if (!SetDate(false, stringTokens, intTokens)) {
        return;
    }
    ClearViewElement(veDateTime);
    DrawViewElement(veDateTime, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawTime(void) {
    if (!ExecuteViewElement(veTime)) {
        return;
    }

    map < string, string > stringTokens;
    map < string, int > intTokens;

    if (!SetTime(false, stringTokens, intTokens)) {
        return;
    }    
    ClearViewElement(veTime);
    DrawViewElement(veTime, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawTitle(const cRecording *recording) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    const char *recName = NULL;
    const cRecordingInfo *recInfo = recording->Info();
    if (recInfo) {
        recName = recInfo->Title();
    }
    if (!recName)
        recName = recording->Name();
    string recShortText = recInfo->ShortText() ? recInfo->ShortText() : "";
    string recDate = *ShortDateString(recording->Start());
    string recTime = *TimeString(recording->Start());

    stringTokens.insert(pair<string,string>("rectitle", recName ? recName : ""));
    stringTokens.insert(pair<string,string>("recsubtitle", recShortText));
    stringTokens.insert(pair<string,string>("recdate", recDate));
    stringTokens.insert(pair<string,string>("rectime", recTime));

    DrawViewElement(veRecTitle, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawTitle(const char *title) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    stringTokens.insert(pair<string,string>("rectitle", title));
    stringTokens.insert(pair<string,string>("recsubtitle", ""));
    stringTokens.insert(pair<string,string>("recdate", ""));
    stringTokens.insert(pair<string,string>("rectime", ""));

    DrawViewElement(veRecTitle, &stringTokens, &intTokens);
}


void cDisplayReplayView::DrawRecordingInformation(const cRecording *recording) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;
    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);
    bool isHD = false;
    string resName = GetScreenResolutionString(screenWidth, screenHeight, &isHD);
    bool isWideScreen = false;
    string aspectName = GetScreenAspectString(aspect, &isWideScreen);

    intTokens.insert(pair<string,int>("screenwidth", screenWidth));
    intTokens.insert(pair<string,int>("screenheight", screenHeight));
    intTokens.insert(pair<string,int>("isHD", isHD));
    intTokens.insert(pair<string,int>("isWideScreen", isWideScreen));
    stringTokens.insert(pair<string,string>("resolution", resName));
    stringTokens.insert(pair<string,string>("aspect", aspectName));

    ClearViewElement(veRecInfo);
    DrawViewElement(veRecInfo, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawScraperContent(const cRecording *recording) {
    if (!recording)
        return;

    if (!ExecuteViewElement(veScraperContent)) {
        return;
    }

    if (DetachViewElement(veScraperContent)) {
        cViewElementScraperContent *viewElement = dynamic_cast<cViewElementScraperContent*>(GetViewElement(veScraperContent));
        if (!viewElement) {
            viewElement = new cViewElementScraperContent(NULL, recording, tmplView->GetViewElement(veScraperContent));
            AddViewElement(veScraperContent, viewElement);
            viewElement->Start();
        } else {
            if (!viewElement->Starting()) {
                viewElement->SetRecording(recording);
                viewElement->Render();
            }
        }
    } else {
        map < string, string > stringTokens;
        map < string, int > intTokens;

        SetPosterBannerV2(recording, stringTokens, intTokens);
        ClearViewElement(veScraperContent);
        DrawViewElement(veScraperContent, &stringTokens, &intTokens);
    }
}

void cDisplayReplayView::DrawCurrent(const char *current) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    stringTokens.insert(pair<string,string>("reccurrent", current));

    ClearViewElement(veRecCurrent);
    DrawViewElement(veRecCurrent, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawTotal(const char *total) {
    map < string, string > stringTokens;
    map < string, int > intTokens;
    intTokens.insert(pair<string,int>("timeshift", timeShiftActive));
    stringTokens.insert(pair<string,string>("rectotal", total));
    stringTokens.insert(pair<string,string>("timeshifttotal", timeShiftDuration));

    ClearViewElement(veRecTotal);
    DrawViewElement(veRecTotal, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawEndTime(int current, int total) {
    if (!current)
        return;
    int totalLength = total;
    int recordingLength = length;
    if (timeShiftActive && timeShiftFramesTotal > 0) {
        totalLength = timeShiftFramesTotal;
        recordingLength = timeShiftLength;
    }
    double rest = (double)(totalLength - current) / (double)totalLength;
    time_t end = time(0) + rest*recordingLength;
    string endTime = *TimeString(end);
    if (!endTime.compare(endLast)) {
        return;
    }
    endLast = endTime;

    map < string, string > stringTokens;
    map < string, int > intTokens;
    stringTokens.insert(pair<string,string>("recend", endTime));

    ClearViewElement(veRecEnd);
    DrawViewElement(veRecEnd, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawProgressBar(int current, int total) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    intTokens.insert(pair<string,int>("current", current));
    intTokens.insert(pair<string,int>("total", total));
    intTokens.insert(pair<string,int>("timeshift", timeShiftActive));
    if (timeShiftActive) {
        intTokens.insert(pair<string,int>("timeshifttotal", timeShiftFramesTotal));
    }

    ClearViewElement(veRecProgressBar);
    DrawViewElement(veRecProgressBar, &stringTokens, &intTokens);    
}

void cDisplayReplayView::DrawMarks(const cMarks *marks, int current, int total) {
    if (!marks)
        return;
    if (!MarksChanged(marks, current))
        return;

    map < string, string > stringTokens;
    map < string, int > intTokens;
    intTokens.insert(pair<string,int>("timeshift", timeShiftActive));

    map < string, vector< map< string, string > > > loopTokens;
    vector< map< string, string > > markTokens;
    stringstream tot;
    tot << total;
    stringstream timeshifttot;
    timeshifttot << timeShiftFramesTotal;

    bool isStartMark = true;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        map< string, string > markVals;
        stringstream pos;
        pos << m->Position();
        markVals.insert(pair< string, string >("marks[position]", pos.str()));
        markVals.insert(pair< string, string >("marks[total]", tot.str()));
        if (timeShiftActive) {
            markVals.insert(pair< string, string >("marks[timeshifttotal]", timeshifttot.str()));
        }
        markVals.insert(pair< string, string >("marks[startmark]", isStartMark ? "1" : "0"));
        markVals.insert(pair< string, string >("marks[active]", (m->Position() == current) ? "1" : "0"));
        const cMark *m2 = marks->Next(m);
        if (m2) { 
            stringstream posNext;
            posNext << m2->Position();
            markVals.insert(pair< string, string >("marks[endposition]", posNext.str()));
        } else {
            markVals.insert(pair< string, string >("marks[endposition]", tot.str()));            
        }
        isStartMark = !isStartMark;
        markTokens.push_back(markVals);
    }
    loopTokens.insert(pair< string, vector< map< string, string > > >("marks", markTokens));

    ClearViewElement(veCuttingMarks);
    DrawViewElement(veCuttingMarks, &stringTokens, &intTokens, &loopTokens);
}

void cDisplayReplayView::DrawControlIcons(bool play, bool forward, int speed, bool modeOnly) {
    map < string, string > stringTokens;
    map < string, int > intTokens;

    bool isPlay = false;
    bool isPause = false;
    bool isFF = false;
    bool isFF1x = false;
    bool isFF2x = false;
    bool isFF3x = false;
    bool isRew = false;
    bool isRew1x = false;
    bool isRew2x = false;
    bool isRew3x = false;

    if (speed == -1) {
        if (play) {
            isPlay = true;
        } else {
            isPause = true;
        }
    } else if (forward) {
        if (!play) {
            isPause = true;
        }
        if (speed == 1) {
            isFF1x = true;
        } else if (speed == 2) {
            isFF2x = true;
        } else if (speed == 3) {
            isFF3x = true;
        } else {
            isFF = true;
        }
    } else {
        if (!play) {
            isPause = true;
        }
        if (speed == 1) {
            isRew1x = true;
        } else if (speed == 2) {
            isRew2x = true;
        } else if (speed == 3) {
            isRew3x = true;
        } else {
            isRew = true;
        }
    }
    intTokens.insert(pair<string,int>("play", isPlay));
    intTokens.insert(pair<string,int>("pause", isPause));
    intTokens.insert(pair<string,int>("forward", isFF));
    intTokens.insert(pair<string,int>("forward1x", isFF1x));
    intTokens.insert(pair<string,int>("forward2x", isFF2x));
    intTokens.insert(pair<string,int>("forward3x", isFF3x));
    intTokens.insert(pair<string,int>("rewind", isRew));
    intTokens.insert(pair<string,int>("rewind1x", isRew1x));
    intTokens.insert(pair<string,int>("rewind2x", isRew2x));
    intTokens.insert(pair<string,int>("rewind3x", isRew3x));

    if (modeOnly) {
        ClearViewElement(veControlIconsModeOnly);
        DrawViewElement(veControlIconsModeOnly, &stringTokens, &intTokens);
    } else {
        ClearViewElement(veControlIcons);
        DrawViewElement(veControlIcons, &stringTokens, &intTokens);
    }
}

void cDisplayReplayView::DrawProgressModeOnly(double fps, int current, int total) {
    string cur = GetTimeString((double)current / fps);
    string tot = GetTimeString((double)total / fps);
    map < string, string > stringTokens;
    map < string, int > intTokens;
    intTokens.insert(pair<string,int>("current", current));
    intTokens.insert(pair<string,int>("total", total));
    stringTokens.insert(pair<string,string>("timecurrent", cur));
    stringTokens.insert(pair<string,string>("timetotal", tot));
    ClearViewElement(veProgressModeOnly);
    DrawViewElement(veProgressModeOnly, &stringTokens, &intTokens);
}


void cDisplayReplayView::DrawJump(const char *jump) {
    if (!jump) {
        ClearViewElement(veRecJump);
        return;
    }
    
    map < string, string > stringTokens;
    map < string, int > intTokens;
    stringTokens.insert(pair<string,string>("jump", jump));

    ClearViewElement(veRecJump);
    DrawViewElement(veRecJump, &stringTokens, &intTokens);        
}

void cDisplayReplayView::DrawMessage(eMessageType type, const char *text) {
    if (!text) {
        ClearViewElement(veMessage);
        return;
    }
    
    map < string, string > stringTokens;
    map < string, int > intTokens;
    
    stringTokens.insert(pair<string,string>("text", text));

    intTokens.insert(pair<string,int>("status",  (type == mtStatus)  ? true : false));
    intTokens.insert(pair<string,int>("info",    (type == mtInfo)    ? true : false));
    intTokens.insert(pair<string,int>("warning", (type == mtWarning) ? true : false));
    intTokens.insert(pair<string,int>("error",   (type == mtError)   ? true : false));
    
    ClearViewElement(veMessage);
    DrawViewElement(veMessage, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawOnPause(string recFileName, bool modeOnly) {
    eViewElement veTmplOnPause = modeOnly ? veOnPauseModeOnly : veOnPause;
    cTemplateViewElement *tmplOnPause = tmplView->GetViewElement(veTmplOnPause);
    if (!tmplOnPause)
        return;
    onPauseView = new cDisplayReplayOnPauseView(recFileName, tmplOnPause);
    onPauseView->Start();
}

void cDisplayReplayView::ClearOnPause(void) {
    if (onPauseView) {
        delete onPauseView;
        onPauseView = NULL;
    }
}

void cDisplayReplayView::DelayOnPause(void) {
    if (onPauseView) {
        onPauseView->ResetSleep();
    }
}

void cDisplayReplayView::DrawCustomTokens(void) {
    if (!ExecuteViewElement(veCustomTokens)) {
        return;
    }
    if (!tmplView)
        return;
    map < string, string > stringTokens = tmplView->GetCustomStringTokens();
    map < string, int > intTokens = tmplView->GetCustomIntTokens();
    ClearViewElement(veCustomTokens);
    DrawViewElement(veCustomTokens, &stringTokens, &intTokens);
}

bool cDisplayReplayView::CustomTokenChange(void) {
    if (!tmplView)
        return false;
    return tmplView->CustomTokenChange();
}

/****************************************************************************************
* Private Functions
*****************************************************************************************/

bool cDisplayReplayView::MarksChanged(const cMarks *marks, int current) {
    if (!marks)
        return false;

    bool redraw = false;
    //if mark was active, we redraw always
    if (markActive >= 0) {
        markActive = -1;
        redraw = true;
    }
    //check if current position in recording hits mark exactly
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        if (m->Position() == current) {
            markActive = current;
            redraw = true;
            break;
        }
    }
    if (redraw)
        return true;
    //if number of marks has changed, redraw
    int numMarks = marks->Count();
    if (numMarks != numMarksLast) {
        RememberMarks(marks);
        return true;
    }
    if (!lastMarks)
        return false;
    //if position has changed, redraw
    int i=0;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        if (m->Position() != lastMarks[i]) {
            RememberMarks(marks);
            return true;
        }
        i++;
    }
    return false;
}

void cDisplayReplayView::RememberMarks(const cMarks *marks) {
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
