#include "templateviewgrid.h"
#include "../config.h"
#include "../libcore/helpers.h"

cTemplateViewGrid::cTemplateViewGrid(void) : cTemplateViewElement() {
}

cTemplateViewGrid::~cTemplateViewGrid(void) {
}

bool cTemplateViewGrid::CalculatePixmapParameters(void) {
    bool paramsValid = true;
    int gridX = parameters->GetNumericParameter(ptX);
    int gridY = parameters->GetNumericParameter(ptY);
    int gridWidth = parameters->GetNumericParameter(ptWidth);
    int gridHeight = parameters->GetNumericParameter(ptHeight);

    for (vector<cTemplatePixmap*>::iterator pix = viewPixmaps.begin(); pix != viewPixmaps.end(); pix++) {
        (*pix)->SetContainer(gridX, gridY, gridWidth, gridHeight);
        (*pix)->SetGlobals(globals);
        paramsValid = paramsValid && (*pix)->CalculateParameters();
    }
    return paramsValid;
}

void cTemplateViewGrid::Debug(void) {
    esyslog("skindesigner: --- Grid: ");
    cTemplateViewElement::Debug();
}