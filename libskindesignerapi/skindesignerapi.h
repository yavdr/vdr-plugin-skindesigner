#ifndef __LIBSKINDESIGNERAPI_SERVICES_H
#define __LIBSKINDESIGNERAPI_SERVICES_H

using namespace std;

#include <string>
#include <map>
#include <vector>
#include <vdr/osdbase.h>

namespace skindesignerapi {

class ISDDisplayMenu : public cSkinDisplayMenu {
public:
    virtual void SetTitle(const char *Title) = 0;
    virtual void SetPluginMenu(string name, int menu, int type, bool init) = 0;
    virtual bool SetItemPlugin(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens, int Index, bool Current, bool Selectable) = 0;
    virtual bool SetPluginText(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens) = 0;
};

class ISkinDisplayPlugin {
public:
    virtual ~ISkinDisplayPlugin(void) {};
    virtual void Deactivate(bool hide) = 0;
    virtual void Activate(void) = 0;
    virtual void ClearViewElement(int id) = 0;
    virtual void DisplayViewElement(int id) = 0;
    virtual void SetViewElementIntTokens(map<string,int> *intTokens) = 0;
    virtual void SetViewElementStringTokens(map<string,string> *stringTokens) = 0;
    virtual void SetViewElementLoopTokens(map<string,vector<map<string,string> > > *loopTokens) = 0;
    virtual void InitGrids(int viewGridID) = 0;
    virtual void SetGrid(int viewGridID, long gridID, double x, double y, double width, double height, map<string,int> *intTokens, map<string,string> *stringTokens) = 0;
    virtual void SetGridCurrent(int viewGridID, long gridID, bool current) = 0;
    virtual void DeleteGrid(int viewGridID, long gridID) = 0;
    virtual void DisplayGrids(int viewGridID) = 0;
    virtual void ClearGrids(int viewGridID) = 0;
    virtual void SetTabIntTokens(map<string,int> *intTokens) = 0;
    virtual void SetTabStringTokens(map<string,string> *stringTokens) = 0;
    virtual void SetTabLoopTokens(map<string,vector<map<string,string> > > *loopTokens) = 0;
    virtual void SetTabs(void) = 0;
    virtual void TabLeft(void) = 0;
    virtual void TabRight(void) = 0;
    virtual void TabUp(void) = 0;
    virtual void TabDown(void) = 0;
    virtual void DisplayTabs(void) = 0;
    virtual void Flush(void) = 0;
    virtual bool ChannelLogoExists(string channelId) = 0;
    virtual string GetEpgImagePath(void) = 0;
};

class cPluginStructure {
public:
    cPluginStructure(void) {
        name = "";
    };
    void SetMenu(int key, string templateName) {
        menus.insert(pair<int, string>(key, templateName));
    }
    void SetView(int key, string templateName) {
        views.insert(pair<int, string>(key, templateName));     
    }
    void SetSubView(int view, int subView, string templateName) {
        pair<int, string> sub = make_pair(subView, templateName);
        subViews.insert(pair<int, pair<int, string> >(view, sub));     
    }
    void SetViewElement(int view, int viewElement, string name) {
        map< int, map<int, string> >::iterator hit = viewElements.find(view);
        if (hit == viewElements.end()) {
            map<int, string> vE;
            vE.insert(pair<int, string >(viewElement, name));
            viewElements.insert(pair<int, map < int, string > >(view, vE));
        } else {
            (hit->second).insert(pair<int, string >(viewElement, name));
        }
    }
    void SetViewGrid(int view, int viewGrid, string name) {
        map< int, map<int, string> >::iterator hit = viewGrids.find(view);
        if (hit == viewGrids.end()) {
            map<int, string> vG;
            vG.insert(pair<int, string >(viewGrid, name));
            viewGrids.insert(pair<int, map < int, string > >(view, vG));
        } else {
            (hit->second).insert(pair<int, string >(viewGrid, name));
        }
    }
    string name;                                     //name of plugin
    map< int, string > menus;                        //menus as key -> templatename hashmap 
    map< int, string>  views;                        //standalone views as key -> templatename hashmap 
    multimap< int, pair <int, string> >  subViews;   //subviews of standalone views as view -> (subview, templatename) multimap 
    map< int, map <int, string> > viewElements;      //viewelements as key -> (viewelement, viewelementname) hashmap 
    map< int, map <int, string> > viewGrids;         //viewgrids as key -> (viewgrid, viewgridname) hashmap
};

class SkindesignerAPI {
private:
    static SkindesignerAPI* skindesigner;
protected:
    SkindesignerAPI(void);
    virtual ~SkindesignerAPI(void);
    virtual bool ServiceRegisterPlugin(cPluginStructure *plugStructure) = 0;
    virtual ISDDisplayMenu *ServiceGetDisplayMenu(void) = 0;
    virtual ISkinDisplayPlugin *ServiceGetDisplayPlugin(string pluginName, int viewID, int subViewID) = 0;
public:
    static bool RegisterPlugin(cPluginStructure *plugStructure);
    static ISDDisplayMenu *GetDisplayMenu(void);
    static ISkinDisplayPlugin *GetDisplayPlugin(string pluginName, int viewID, int subViewID);
};

}

#endif //__LIBSKINDESIGNERAPI_SERVICES_H
