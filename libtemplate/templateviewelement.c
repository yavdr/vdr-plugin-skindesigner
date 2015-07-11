#include "templateviewelement.h"
#include "../config.h"

cTemplateViewElement::cTemplateViewElement(void) {
    debugTokens = false;
    parameters = NULL;
    containerX = 0;
    containerY = 0;
    containerWidth = 0;
    containerHeight = 0;
    pixOffset = -1;
    pixmapIterator = NULL;
    currentNode = NULL;
}

cTemplateViewElement::~cTemplateViewElement(void) {
    if (parameters)
        delete parameters;
    for (vector<cTemplatePixmapNode*>::iterator it = viewPixmapNodes.begin(); it != viewPixmapNodes.end(); it++) {
        delete (*it);
    }
}

void cTemplateViewElement::SetContainer(int x, int y, int width, int height) { 
    containerX = x;
    containerY = y;
    containerWidth = width; 
    containerHeight = height; 
}

void cTemplateViewElement::SetGlobals(cGlobals *globals) { 
    this->globals = globals;
    for (vector<cTemplatePixmapNode*>::iterator pix = viewPixmapNodes.begin(); pix != viewPixmapNodes.end(); pix++) {
        (*pix)->SetGlobals(globals);
    }
}

void cTemplateViewElement::SetParameters(vector<pair<string, string> > &params) {
    parameters = new cTemplateFunction(ftViewElement);
    parameters->SetGlobals(globals);
    parameters->SetParameters(params);
}

bool cTemplateViewElement::CalculateParameters(void) {
    if (!parameters)
        return true;
    bool paramsValid = true;
    parameters->SetContainer(containerX, containerY, containerWidth, containerHeight);
    parameters->SetGlobals(globals);
    paramsValid = parameters->CalculateParameters();
    parameters->ParseParameters();
    return paramsValid;
}

bool cTemplateViewElement::CalculatePixmapParameters(void) {
    bool paramsValid = true;
    for (vector<cTemplatePixmapNode*>::iterator pix = viewPixmapNodes.begin(); pix != viewPixmapNodes.end(); pix++) {
        (*pix)->SetContainer(containerX, containerY, containerWidth, containerHeight);
        (*pix)->SetGlobals(globals);
        paramsValid = paramsValid && (*pix)->CalculateParameters();
    }
    return paramsValid;
}

bool cTemplateViewElement::CalculatePixmapParametersList(int orientation, int numElements) {
    bool paramsValid = true;
    for (vector<cTemplatePixmapNode*>::iterator pix = viewPixmapNodes.begin(); pix != viewPixmapNodes.end(); pix++) {
        (*pix)->SetContainer(containerX, containerY, containerWidth, containerHeight);
        (*pix)->SetGlobals(globals);
        if (orientation == orHorizontal) {
            if (numElements > 0) {
                int width = containerWidth / numElements;
                (*pix)->SetWidth(width);
            }
        } else if (orientation == orVertical) {
            if (numElements > 0) {
                int height = containerHeight / numElements;
                (*pix)->SetHeight(height);
            }
        } 
        paramsValid = paramsValid && (*pix)->CalculateParameters();
    }
    return paramsValid;
}

int cTemplateViewElement::GetNumericParameter(eParamType type) {
    if (!parameters)
        return -1;
    return parameters->GetNumericParameter(type);
}

int cTemplateViewElement::GetNumPixmaps(void) {
    int numPixmaps = 0;
    for (vector<cTemplatePixmapNode*>::iterator pix = viewPixmapNodes.begin(); pix != viewPixmapNodes.end(); pix++) {
        numPixmaps += (*pix)->NumPixmaps();
    }
    return numPixmaps;
};

void cTemplateViewElement::InitPixmapNodeIterator(void) {
    pixmapNodeIterator = viewPixmapNodes.begin();
}

cTemplatePixmapNode *cTemplateViewElement::GetNextPixmapNode(void) {
    if (pixmapNodeIterator == viewPixmapNodes.end())
        return NULL;
    cTemplatePixmapNode *pix = *pixmapNodeIterator;
    pixmapNodeIterator++;
    return pix;
}

void cTemplateViewElement::InitPixmapIterator(void) {
    pixmapNodeIterator = viewPixmapNodes.begin();
    if (pixmapNodeIterator == viewPixmapNodes.end())
        return;
    if (!(*pixmapNodeIterator)->IsContainer()) {
        //first node is a pixmap, use this
        pixmapIterator = dynamic_cast<cTemplatePixmap*>(*pixmapNodeIterator);
        return;
    } 
    //first node is a container, so fetch first pixmap of this container
    currentNode = dynamic_cast<cTemplatePixmapContainer*>(*pixmapNodeIterator);
    currentNode->InitIterator();
    pixmapIterator = currentNode->GetNextPixmap();
}

cTemplatePixmap *cTemplateViewElement::GetNextPixmap(void) {
    if (!pixmapIterator)
        return NULL;
    cTemplatePixmap *current = pixmapIterator;
    //set next pixmap
    if (!currentNode) {
        //last node was a pixmap
        pixmapNodeIterator++;
        if (pixmapNodeIterator == viewPixmapNodes.end()) {
            pixmapIterator = NULL;
            return current;
        }
        if (!(*pixmapNodeIterator)->IsContainer()) {
            //node is a pixmap, use this
            pixmapIterator = dynamic_cast<cTemplatePixmap*>(*pixmapNodeIterator);
            return current;
        }
        //node is a container, so fetch first pixmap of this container
        currentNode = dynamic_cast<cTemplatePixmapContainer*>(*pixmapNodeIterator);
        currentNode->InitIterator();
        pixmapIterator = currentNode->GetNextPixmap();
    } else {
        pixmapIterator = currentNode->GetNextPixmap();
        if (pixmapIterator) {
            return current;
        }
        currentNode = NULL;
        pixmapNodeIterator++;
        if (pixmapNodeIterator == viewPixmapNodes.end()) {
            pixmapIterator = NULL;
            return current;
        }
        if (!(*pixmapNodeIterator)->IsContainer()) {
            //node is a pixmap, use this
            pixmapIterator = dynamic_cast<cTemplatePixmap*>(*pixmapNodeIterator);
            return current;
        }
        //node is a container, so fetch first pixmap of this container
        currentNode = dynamic_cast<cTemplatePixmapContainer*>(*pixmapNodeIterator);
        currentNode->InitIterator();
        pixmapIterator = currentNode->GetNextPixmap();
    }
    return current;
}

cTemplateFunction *cTemplateViewElement::GetFunction(string name) {
    InitPixmapIterator();
    cTemplatePixmap *pix = NULL;
    while (pix = GetNextPixmap()) {
        pix->InitFunctionIterator();
        cTemplateFunction *func = NULL;
        while(func = pix->GetNextFunction()) {
            if (func->GetType() == ftDrawText) {
                string funcName = func->GetParameter(ptName);
                if (!funcName.compare(name))
                    return func;
            } else {
                continue;
            }
        }
    }
    return NULL;
}

bool cTemplateViewElement::Execute(void) {
    if (!parameters)
        return true;
    return parameters->DoExecute();
}

bool cTemplateViewElement::Detach(void) {
    if (!parameters)
        return false;
    int detached = parameters->GetNumericParameter(ptDetached);
    if (detached == 1)
        return true;
    return false;
}

string cTemplateViewElement::GetMode(void) {
    if (!parameters)
        return "";
    return parameters->GetParameter(ptMode);
}

bool cTemplateViewElement::DebugTokens(void) {
    if (!parameters)
        return false;
    return parameters->DoDebug();
}


void cTemplateViewElement::Debug(void) {
    esyslog("skindesigner: viewelement container size x: %d, y: %d, width: %d, height %d", containerX, containerY, containerWidth, containerHeight);
    if (parameters)
        parameters->Debug();
    return;
    for (vector<cTemplatePixmapNode*>::iterator it = viewPixmapNodes.begin(); it != viewPixmapNodes.end(); it++) {
        (*it)->Debug();
    }
}