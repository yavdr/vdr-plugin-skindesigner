#ifndef __DISPLAYPLUGIN_H
#define __DISPLAYPLUGIN_H

#include "libskindesignerapi/skindesignerapi.h"
#include "libtemplate/template.h"
#include "views/displaypluginview.h"

class cSkinDisplayPlugin : public skindesignerapi::ISkinDisplayPlugin {
private:
    bool doOutput;
    bool initial;
    cDisplayPluginView *pluginView;
public:
    cSkinDisplayPlugin(void) {};
    cSkinDisplayPlugin(cTemplate *pluginTemplate, int subViewID);
    virtual ~cSkinDisplayPlugin(void);
    virtual void Deactivate(bool hide);
    virtual void Activate(void);
    virtual void ClearViewElement(int id);
    virtual void DisplayViewElement(int id);
    virtual void SetViewElementIntTokens(map<string,int> *intTokens);
    virtual void SetViewElementStringTokens(map<string,string> *stringTokens);
    virtual void SetViewElementLoopTokens(map<string,vector<map<string,string> > > *loopTokens);
    virtual void InitGrids(int viewGridID);
    virtual void SetGrid(int viewGridID, long gridID, double x, double y, double width, double height, map<string,int> *intTokens, map<string,string> *stringTokens);
    virtual void SetGridCurrent(int viewGridID, long gridID, bool current);
    virtual void DeleteGrid(int viewGridID, long gridID);
    virtual void DisplayGrids(int viewGridID);
    virtual void ClearGrids(int viewGridID);
    virtual void SetTabIntTokens(map<string,int> *intTokens);
    virtual void SetTabStringTokens(map<string,string> *stringTokens);
    virtual void SetTabLoopTokens(map<string,vector<map<string,string> > > *loopTokens);
    virtual void SetTabs(void);
    virtual void TabLeft(void);
    virtual void TabRight(void);
    virtual void TabUp(void);
    virtual void TabDown(void);
    virtual void DisplayTabs(void);
    virtual void Flush(void);
    virtual bool ChannelLogoExists(string channelId);
    virtual string GetEpgImagePath(void);
};

#endif //__DISPLAYPLUGIN_H
