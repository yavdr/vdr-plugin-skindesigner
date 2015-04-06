#ifndef __TEMPLATEVIEWGRID_H
#define __TEMPLATEVIEWGRID_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "templateviewelement.h"

using namespace std;

// --- cTemplateViewGrid -------------------------------------------------------------

class cTemplateViewGrid : public cTemplateViewElement {
private:
public:
    cTemplateViewGrid(void);
    ~cTemplateViewGrid(void);
    bool CalculatePixmapParameters(void);
    void Debug(void);
};

#endif //__TEMPLATEVIEWGRID_H