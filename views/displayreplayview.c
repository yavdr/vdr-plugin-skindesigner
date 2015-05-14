#define __STL_CONFIG_H
#include <vdr/menu.h>
#include "../services/scraper2vdr.h"
#include "displayreplayview.h"
#include "../libcore/helpers.h"

cDisplayReplayView::cDisplayReplayView(cTemplateView *tmplView) : cView(tmplView) {
    length = 0;
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

    int mediaWidth = 0;
    int mediaHeight = 0;
    string mediaPath = "";
    bool isBanner = false;
    int posterWidth = 0;
    int posterHeight = 0;
    string posterPath = "";
    bool hasPoster = false;
    int bannerWidth = 0;
    int bannerHeight = 0;
    string bannerPath = "";
    bool hasBanner = false;

    static cPlugin *pScraper = GetScraperPlugin();
    if (pScraper) {
        ScraperGetPosterBannerV2 call;
        call.event = NULL;
        call.recording = recording;
        if (pScraper->Service("GetPosterBannerV2", &call)) {
            if ((call.type == tSeries) && call.banner.path.size() > 0) {
                mediaWidth = call.banner.width;
                mediaHeight = call.banner.height;
                mediaPath = call.banner.path;
                isBanner = true;
                bannerWidth = mediaWidth;
                bannerHeight = mediaHeight;
                bannerPath = mediaPath;
                hasBanner = true;

                ScraperGetPoster callPoster;
                callPoster.event = NULL;
                callPoster.recording = recording;
                if (pScraper->Service("GetPoster", &callPoster)) {
                    posterWidth = callPoster.poster.width;
                    posterHeight = callPoster.poster.height;
                    posterPath = callPoster.poster.path;
                    hasPoster = true;
                }
            } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
                mediaWidth = call.poster.width;
                mediaHeight = call.poster.height;
                mediaPath = call.poster.path;
                posterWidth = call.poster.width;
                posterHeight = call.poster.height;
                posterPath = call.poster.path;
                hasPoster = true;
            }
        }
    }
    map < string, int > intTokens;
    map < string, string > stringTokens;
    intTokens.insert(pair<string,int>("mediawidth", mediaWidth));
    intTokens.insert(pair<string,int>("mediaheight", mediaHeight));
    intTokens.insert(pair<string,int>("isbanner", isBanner));
    stringTokens.insert(pair<string,string>("mediapath", mediaPath));
    intTokens.insert(pair<string,int>("posterwidth", posterWidth));
    intTokens.insert(pair<string,int>("posterheight", posterHeight));
    stringTokens.insert(pair<string,string>("posterpath", posterPath));
    intTokens.insert(pair<string,int>("hasposter", hasPoster));
    intTokens.insert(pair<string,int>("bannerwidth", bannerWidth));
    intTokens.insert(pair<string,int>("bannerheight", bannerHeight));
    stringTokens.insert(pair<string,string>("bannerpath", bannerPath));
    intTokens.insert(pair<string,int>("hasbanner", hasBanner));
    ClearViewElement(veScraperContent);
    DrawViewElement(veScraperContent, &stringTokens, &intTokens);
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
    stringTokens.insert(pair<string,string>("rectotal", total));

    ClearViewElement(veRecTotal);
    DrawViewElement(veRecTotal, &stringTokens, &intTokens);
}

void cDisplayReplayView::DrawEndTime(int current, int total) {
    if (!current)
        return;
    double rest = (double)(total - current) / (double)total;
    time_t end = time(0) + rest*length;
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
    stringTokens.insert(pair<string,string>("dummy", ""));
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
    map < string, vector< map< string, string > > > loopTokens;
    vector< map< string, string > > markTokens;
    stringstream tot;
    tot << total;

    bool isStartMark = true;
    for (const cMark *m = marks->First(); m; m = marks->Next(m)) {
        map< string, string > markVals;
        stringstream pos;
        pos << m->Position();
        markVals.insert(pair< string, string >("marks[position]", pos.str()));
        markVals.insert(pair< string, string >("marks[total]", tot.str()));
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
    DrawViewElement(veCustomTokens, &stringTokens, &intTokens);
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
