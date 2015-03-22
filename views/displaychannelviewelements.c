#include "displaychannelviewelements.h"

cViewElementDevices::cViewElementDevices(cTemplateViewElement *tmplViewElement) : cViewElement(tmplViewElement) {
    init = true;
}

void cViewElementDevices::Render(void) {
    map < string, vector< map< string, string > > > deviceLoopTokens;
    vector< map< string, string > > devices;

    if (init)
        InitDevices();
    bool changed = SetDevices(init, &intTokens, &devices);
    init = false;
    
    if (!changed)
        return;

    deviceLoopTokens.insert(pair< string, vector< map< string, string > > >("devices", devices));
    
    Clear();
    Draw(&deviceLoopTokens);
}