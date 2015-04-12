#include "config.h"
#include "displayplugin.h"

cSkinDisplayPlugin::cSkinDisplayPlugin(cTemplate *pluginTemplate, int subViewID) {
    if (!pluginTemplate) {
        doOutput = false;
        return;
    } else {
        doOutput = true;
    }
    initial = true;
    if (subViewID > -1) {
        cTemplateView *subView = pluginTemplate->GetRootView()->GetSubView((eSubView)subViewID);
        if (!subView) {
            doOutput = false;
            return;            
        }
        pluginView = new cDisplayPluginView(subView, false);
    } else {
        pluginView = new cDisplayPluginView(pluginTemplate->GetRootView(), true);
    }
    if (!pluginView->createOsd() && subViewID < 0) {
        doOutput = false;
        return;
    }
    pluginView->DrawDebugGrid();
}

cSkinDisplayPlugin::~cSkinDisplayPlugin(void) {
    if (pluginView) {
        delete pluginView;
        pluginView = NULL;
    }
}

void cSkinDisplayPlugin::Deactivate(bool hide) {
    if (!doOutput) {
        return;
    }
    pluginView->Deactivate(hide);
}

void cSkinDisplayPlugin::Activate(void) {
    if (!doOutput) {
        return;
    }
    pluginView->Activate();
}


void cSkinDisplayPlugin::ClearViewElement(int id) {
    if (!doOutput) {
        return;
    }
    pluginView->CleanViewElement(id);
}

void cSkinDisplayPlugin::DisplayViewElement(int id) {
    if (!doOutput) {
        return;
    }
    pluginView->DisplayViewElement(id);
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

void cSkinDisplayPlugin::SetTabIntTokens(map<string,int> *intTokens) {
    if (!doOutput) {
        return;
    }
    pluginView->SetTabIntTokens(intTokens);
}

void cSkinDisplayPlugin::SetTabStringTokens(map<string,string> *stringTokens) {
    if (!doOutput) {
        return;
    }
    pluginView->SetTabStringTokens(stringTokens);
}

void cSkinDisplayPlugin::SetTabLoopTokens(map<string,vector<map<string,string> > > *loopTokens) {
    if (!doOutput) {
        return;
    }
    pluginView->SetTabLoopTokens(loopTokens);
}

void cSkinDisplayPlugin::SetTabs(void) {
    if (!doOutput) {
        return;
    }
    pluginView->SetTabs();    
}

void cSkinDisplayPlugin::TabLeft(void) {
    if (!doOutput) {
        return;
    }
    pluginView->TabLeft();
}

void cSkinDisplayPlugin::TabRight(void) {
    if (!doOutput) {
        return;
    }
    pluginView->TabRight();
}

void cSkinDisplayPlugin::TabUp(void) {
    if (!doOutput) {
        return;
    }
    pluginView->TabUp();
}

void cSkinDisplayPlugin::TabDown(void) {
    if (!doOutput) {
        return;
    }
    pluginView->TabDown();
}

void cSkinDisplayPlugin::DisplayTabs(void) {
        if (!doOutput) {
        return;
    }
    pluginView->DisplayTab(); 
}

void cSkinDisplayPlugin::Flush(void) {
    if (initial) {
        pluginView->DoStart();
        initial = false;
    }
    pluginView->Flush();
}

bool cSkinDisplayPlugin::ChannelLogoExists(string channelId) {
    return imgCache->LogoExists(channelId);
}

string cSkinDisplayPlugin::GetEpgImagePath(void) {
    return *config.epgImagePath;
}