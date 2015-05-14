#ifndef __TEMPLATEVIEWELEMENT_H
#define __TEMPLATEVIEWELEMENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "templatepixmap.h"
#include "templatefunction.h"

using namespace std;

// --- cTemplateViewElement -------------------------------------------------------------

enum eViewElement {
    //Common ViewElements
    veUndefined,
    veBackground,
    veDateTime,
    veTime,
    veMessage,
    veSortMode,
    veDevices,
    veCurrentWeather,
    veCustomTokens,
    //DisplayChannel ViewElements
    veChannelInfo,
    veChannelGroup,
    veEpgInfo,
    veProgressBar,
    veProgressBarBack,
    veStatusInfo,
    veAudioInfo,
    veScreenResolution,
    veSignalQuality,
    veSignalQualityBack,
    veScraperContent,
    veEcmInfo,
    //DisplayMenu ViewElements
    veHeader,
    veButtons,
    veDiscUsage,
    veSystemLoad,
    veSystemMemory,
    veVDRStats,
    veTemperatures,
    veTimers,
    veLastRecordings,
    veCurrentSchedule,
    veMenuItem,
    veMenuCurrentItemDetail,
    veScrollbar,
    veDetailHeader,
    veTabLabels,
    //DisplayReplay ViewElements
    veRecTitle,
    veRecInfo,
    veRecCurrent,
    veRecTotal,
    veRecEnd,
    veRecProgressBar,
    veCuttingMarks,
    veControlIcons,
    veControlIconsModeOnly,
    veBackgroundModeOnly,
    veRecJump,
    veOnPause,
    veOnPauseModeOnly,
    //DisplayVolume ViewElements
    veVolume
};

enum ePluginInteralViewElements {
    pveScrollbar = -1,
    pveTablabels = -2,
    pveUndefined = 0
};

class cTemplateViewElement {
protected:
    bool debugTokens;
    cGlobals *globals;
    cTemplateFunction *parameters;
    int containerX;
    int containerY;
    int containerWidth;
    int containerHeight;
    vector<cTemplatePixmap*> viewPixmaps;
    vector<cTemplatePixmap*>::iterator pixIterator;
    int pixOffset;
public:
    cTemplateViewElement(void);
    virtual ~cTemplateViewElement(void);
    void SetParameters(vector<pair<string, string> > &params);
    bool CalculateParameters(void);
    virtual bool CalculatePixmapParameters(void);
    bool CalculatePixmapParametersList(int orientation, int numElements);    
    int GetNumericParameter(eParamType type);
    void AddPixmap(cTemplatePixmap *pix) { viewPixmaps.push_back(pix); };
    virtual void SetGlobals(cGlobals *globals);
    void SetContainer(int x, int y, int width, int height);
    void SetPixOffset(int offset) { pixOffset = offset; };
    int GetPixOffset(void) { return pixOffset; };
    virtual int GetNumPixmaps(void) { return viewPixmaps.size(); };
    void InitIterator(void);
    cTemplatePixmap *GetNextPixmap(void);
    cTemplateFunction *GetFunction(string name);
    bool Execute(void);
    bool Detach(void);
    string GetMode(void);
    bool DebugTokens(void);
    virtual void Debug(void);
};

#endif //__TEMPLATEVIEWELEMENT_H