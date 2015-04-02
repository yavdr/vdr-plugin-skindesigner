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
  virtual void SetPluginMenu(string name, int menu, int type, bool init) = 0;
  virtual bool SetItemPlugin(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens, int Index, bool Current, bool Selectable) = 0;
  virtual bool SetPluginText(map<string,string> *stringTokens, map<string,int> *intTokens, map<string,vector<map<string,string> > > *loopTokens) = 0;
};

class SkindesignerAPI {
private:
  static SkindesignerAPI* skindesigner;

protected:
  SkindesignerAPI(void);
  virtual ~SkindesignerAPI(void);

  virtual bool OnRegisterPlugin(string name, map< int, string > menus) = 0;
  virtual ISDDisplayMenu*  OnGetDisplayMenu() = 0;

public:
  static bool CallRegisterPlugin(string name, map< int, string > menus);
  static ISDDisplayMenu* CallGetDisplayMenu();
};

}

#endif //__LIBSKINDESIGNERAPI_SERVICES_H
