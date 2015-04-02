#ifndef __OSDELEMENTS_H
#define __OSDELEMENTS_H

#include <vdr/plugin.h>
#include "skindesignerapi.h"

namespace skindesignerapi {

/**********************************************************************
* cOsdElement
**********************************************************************/

class cOsdElement {
protected:
    ISkinDisplayPlugin *view;
    map < string, string > stringTokens;
    map < string, int > intTokens;
    map < string, vector< map< string, string > > > loopTokens;
public:
    cOsdElement(ISkinDisplayPlugin *view);
    virtual ~cOsdElement();
    void AddLoopToken(string loopName, map<string, string> &tokens);
    void AddStringToken(string key, string value);
    void AddIntToken(string key, int value);
    void ClearTokens(void);
    bool ChannelLogoExists(string channelId);
    string GetEpgImagePath(void);
};

/**********************************************************************
* cViewElement
**********************************************************************/
class cViewElement : public cOsdElement {
private:
    int viewElementID;
public:
    cViewElement(ISkinDisplayPlugin *view, int viewElementID);
    virtual ~cViewElement();
    void Clear(void);
    void Display(void);
};

/**********************************************************************
* cViewGrid
**********************************************************************/
class cViewGrid : public cOsdElement {
private:
    int viewGridID;
public:
    cViewGrid(ISkinDisplayPlugin *view, int viewGridID);
    virtual ~cViewGrid();
    void SetGrid(long gridID, double x, double y, double width, double height);
    void SetCurrent(long gridID, bool current);
    void MoveGrid(long gridID, double x, double y, double width, double height);
    void Delete(long gridID);
    void Clear(void);
    void Display(void);
};

/**********************************************************************
* cViewTab
**********************************************************************/
class cViewTab : public cOsdElement {
private:
public:
    cViewTab(ISkinDisplayPlugin *view);
    virtual ~cViewTab();
    void Init(void);
    void Left(void);
    void Right(void);
    void Up(void);
    void Down(void);
    void Display(void);
};

/**********************************************************************
* cOsdView
**********************************************************************/
class cOsdView {
private:
    ISkinDisplayPlugin *displayPlugin;
public:
    cOsdView(ISkinDisplayPlugin *displayPlugin);
    virtual ~cOsdView();
    void Deactivate(bool hide);
    void Activate(void);
    cViewElement *GetViewElement(int viewElementID);
    cViewGrid *GetViewGrid(int viewGridID);
    cViewTab *GetViewTabs(void);
    void Display(void);
};

}
#endif // __OSDELEMENTS_H