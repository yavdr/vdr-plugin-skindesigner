#ifndef __DISPLAYAUDIOTRACKSVIEW_H
#define __DISPLAYAUDIOTRACKSVIEW_H

#include "../libtemplate/template.h"
#include "displaymenulistview.h"

class cDisplayAudiotracksView : public cView {
protected:
    int numTracks;
    cDisplayMenuListView *listView;
public:
    cDisplayAudiotracksView(int numTracks, cTemplateView *tmplView);
    virtual ~cDisplayAudiotracksView();
    bool createOsd(void);
    void DrawBackground(void);
    void DrawHeader(const char *title, int audioChannel);
    cDisplayMenuListView *GetListView(void) { return listView; };
    void DoFadeIn(void) { Start(); };
    void Flush(void) { DoFlush(); };
    void RenderMenuItems(void);
};

#endif //__DISPLAYAUDIOTRACKSVIEW_H
