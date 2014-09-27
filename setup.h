#ifndef __SKINDESIGNER_SETUP_H
#define __SKINDESIGNER_SETUP_H

#include "config.h"

class cSkinDesignerSetup : public cMenuSetupPage {
    public:
        cSkinDesignerSetup(void);
        virtual ~cSkinDesignerSetup();
    private:
        cDesignerConfig data; 
        void Setup(void);
        virtual eOSState ProcessKey(eKeys Key);
        virtual void Store(void);
};
#endif //__SKINDESIGNER_SETUP_H