#ifndef __DISPLAYPLUGIN_H
#define __DISPLAYPLUGIN_H

#include "libtemplate/template.h"
#include "views/displaypluginview.h"

class cSkinDisplayPlugin {
private:
    bool doOutput;
    bool initial;
    cDisplayPluginView *pluginView;
public:
    cSkinDisplayPlugin(void) {};
    cSkinDisplayPlugin(cTemplate *pluginTemplate);
    virtual ~cSkinDisplayPlugin(void);
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
    virtual void Flush(void);
};

#endif //__DISPLAYPLUGIN_H
