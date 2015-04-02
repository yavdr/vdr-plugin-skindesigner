#include "skindesignerapi.h"

skindesignerapi::SkindesignerAPI *skindesignerapi::SkindesignerAPI::skindesigner = NULL;

skindesignerapi::SkindesignerAPI::SkindesignerAPI(void) {
  if (skindesigner != NULL)
     esyslog("skindesigner should only be loaded once");
  else
     skindesigner = this;
}

skindesignerapi::SkindesignerAPI::~SkindesignerAPI(void) {
  if (skindesigner == this)
     skindesigner = NULL;
}

bool skindesignerapi::SkindesignerAPI::RegisterPlugin(cPluginStructure *plugStructure) {
  if (skindesigner)
     return skindesigner->ServiceRegisterPlugin(plugStructure);
  return false;
}

skindesignerapi::ISDDisplayMenu *skindesignerapi::SkindesignerAPI::GetDisplayMenu() {
  if (skindesigner)
     return skindesigner->ServiceGetDisplayMenu();
  return NULL;
}

skindesignerapi::ISkinDisplayPlugin *skindesignerapi::SkindesignerAPI::GetDisplayPlugin(string pluginName, int viewID, int subViewID) {
  if (skindesigner)
     return skindesigner->ServiceGetDisplayPlugin(pluginName, viewID, subViewID);
  return NULL;
}