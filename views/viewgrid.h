#ifndef __VIEWGRID_H
#define __VIEWGRID_H

#include "string"
#include "map"
#include "view.h"
#include "../libtemplate/templateviewgrid.h"

using namespace std;

class cViewGrid {
private:
    cTemplateViewGrid *tmplGrid;
    map < long, cGrid* > grids;
public:
    cViewGrid(cTemplateViewGrid *tmplGrid);
    virtual ~cViewGrid();
    void SetGrid(long gridID, double x, double y, double width, double height, map<string,int> *intTokens, map<string,string> *stringTokens);
    void SetCurrent(long gridID, bool current);
    void Delete(long gridID);
    void Clear(void);
    void Render(void);
    void Hide(void);
    void Show(void);
    void Debug(void);
};

#endif //__DISPLAYMENULISTVIEW_H