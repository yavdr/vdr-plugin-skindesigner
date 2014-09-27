#ifndef __DISPLAYMENUTABVIEW_H
#define __DISPLAYMENUTABVIEW_H

#include "../libtemplate/template.h"
#include "view.h"

class cDisplayMenuTabView : public cView {
private:
    map < string, string > *stringTokens;
    map < string, int > *intTokens;
    map < string, vector< map< string, string > > > *loopTokens;
    void Action(void);
public:
    cDisplayMenuTabView(cTemplateViewTab *tmplTab);
    virtual ~cDisplayMenuTabView();
    void SetTokens(map < string, int > *intTokens, map < string, string > *stringTokens, map < string, vector< map< string, string > > > *loopTokens);
    void Clear(void);
    void CreateTab(void);
    void Render(void);
    bool KeyUp(void);
    bool KeyDown(void);
    bool KeyLeft(void);
    bool KeyRight(void);
    void GetScrollbarPosition(int &barTop, int &barHeight);
};

#endif //__DISPLAYMENUTABVIEW_H
