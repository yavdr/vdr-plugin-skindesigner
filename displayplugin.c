#include "displayplugin.h"

cSkinDisplayPlugin::cSkinDisplayPlugin(cTemplate *pluginTemplate) {
    if (!pluginTemplate) {
        doOutput = false;
        return;
    } else {
        doOutput = true;
    }
    initial = true;
    pluginView = new cDisplayPluginView(pluginTemplate->GetRootView());
    if (!pluginView->createOsd()) {
        doOutput = false;
        return;
    }
}

cSkinDisplayPlugin::~cSkinDisplayPlugin(void) {
    if (pluginView) {
        delete pluginView;
        pluginView = NULL;
    }
}

void cSkinDisplayPlugin::SetViewElementIntTokens(map<string,int> *intTokens) { 
    if (pluginView)
        pluginView->SetIntTokens(intTokens);
}

void cSkinDisplayPlugin::SetViewElementStringTokens(map<string,string> *stringTokens) { 
    if (pluginView)
        pluginView->SetStringTokens(stringTokens);
}

void cSkinDisplayPlugin::SetViewElementLoopTokens(map<string,vector<map<string,string> > > *loopTokens) { 
    if (pluginView)
        pluginView->SetLoopTokens(loopTokens);
}

void cSkinDisplayPlugin::DisplayViewElement(int id) {
    if (!doOutput) {
        return;
    }
    pluginView->DisplayViewElement(id);
}

void cSkinDisplayPlugin::InitGrids(int viewGridID) {
    if (!doOutput) {
        return;
    }
    pluginView->InitGrids(viewGridID);
}

void cSkinDisplayPlugin::SetGrid(int viewGridID, long gridID, 
                                 double x, double y, double width, double height, 
                                 map<string,int> *intTokens, map<string,string> *stringTokens) {
    if (!doOutput) {
        return;
    }
    pluginView->SetGrid(viewGridID, gridID, x, y, width, height, intTokens, stringTokens);
}

void cSkinDisplayPlugin::SetGridCurrent(int viewGridID, long gridID, bool current) {
    if (!doOutput) {
        return;
    }
    pluginView->SetGridCurrent(viewGridID, gridID, current);
}

void cSkinDisplayPlugin::DeleteGrid(int viewGridID, long gridID) {
    if (!doOutput) {
        return;
    }
    pluginView->DeleteGrid(viewGridID, gridID);   
}

void cSkinDisplayPlugin::DisplayGrids(int viewGridID) {
    if (!doOutput) {
        return;
    }
    pluginView->DisplayGrids(viewGridID);
}

void cSkinDisplayPlugin::ClearGrids(int viewGridID) {
    if (!doOutput) {
        return;
    }
    pluginView->ClearGrids(viewGridID);
}

void cSkinDisplayPlugin::Flush(void) {
    if (initial) {
        pluginView->DoStart();
        initial = false;
    }
    pluginView->Flush();
}