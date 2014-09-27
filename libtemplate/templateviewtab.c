#include "templateviewtab.h"

cTemplateViewTab::cTemplateViewTab(void) : cTemplatePixmap() {
    scrollStep = -1;
}

cTemplateViewTab::~cTemplateViewTab(void) {
}

int cTemplateViewTab::GetScrollStep(void) {
    if (scrollStep > 0)
        return scrollStep;
    int pixWidth = GetNumericParameter(ptWidth);
    int pixHeight = GetNumericParameter(ptHeight);
    string scrollHeight = parameters->GetParameter(ptScrollHeight);

    cNumericParameter p(scrollHeight);
    p.SetAreaSize(pixWidth, pixHeight);
    string parsedValue = "";
    scrollStep = p.Parse(parsedValue);
    if (scrollStep < 1)
        scrollStep = 50;
    return scrollStep;
}

string cTemplateViewTab::GetName(void) {
    return parameters->GetParameter(ptName);
}

void cTemplateViewTab::SetName(string trans) {
    parameters->SetParameter(ptName, trans);
}

void cTemplateViewTab::Debug(void) {
    esyslog("skindesigner: cTemplateViewTab Debug %s", GetName().c_str());
    cTemplatePixmap::Debug();
    esyslog("skindesigner: -------------------------------------------------------");
}
