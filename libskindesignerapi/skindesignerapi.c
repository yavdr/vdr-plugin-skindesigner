#include "skindesignerapi.h"

skindesignerapi::SkindesignerAPI* skindesignerapi::SkindesignerAPI::skindesigner = NULL;

skindesignerapi::SkindesignerAPI::SkindesignerAPI(void)
{
  if (skindesigner != NULL)
     esyslog("skindesigner should only be loaded once");
  else
     skindesigner = this;
}

skindesignerapi::SkindesignerAPI::~SkindesignerAPI(void)
{
  if (skindesigner == this)
     skindesigner = NULL;
}

bool skindesignerapi::SkindesignerAPI::CallRegisterPlugin(string name, map< int, string > menus)
{
  if (skindesigner)
     return skindesigner->OnRegisterPlugin(name, menus);
  return false;
}

skindesignerapi::ISDDisplayMenu*  skindesignerapi::SkindesignerAPI::CallGetDisplayMenu()
{
  if (skindesigner)
     return skindesigner->OnGetDisplayMenu();
  return NULL;
}
