#include "templatepixmap.h"

using namespace std;
// --- cTemplatePixmapNode -------------------------------------------------------------

cTemplatePixmapNode::cTemplatePixmapNode(void) {
    parameters = NULL;
    globals = NULL;
    containerX = 0;
    containerY = 0;
    containerWidth = 0;
    containerHeight = 0;
}

cTemplatePixmapNode::~cTemplatePixmapNode() {
    if (parameters)
        delete parameters;
}

void cTemplatePixmapNode::SetParameters(vector<stringpair> &params) {
    parameters = new cTemplateFunction(isContainer?ftPixmapContainer:ftPixmap);
    parameters->SetGlobals(globals);
    parameters->SetParameters(params);
}

void cTemplatePixmapNode::SetContainer(int x, int y, int w, int h) { 
    containerX = x;
    containerY = y;
    containerWidth = w; 
    containerHeight = h;
}

// --- cTemplatePixmap -------------------------------------------------------------

cTemplatePixmap::cTemplatePixmap(void) {
    pixContainer = NULL;
    isContainer = false;
    scrolling = false;
    background = false;
}

cTemplatePixmap::~cTemplatePixmap() {
    for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
        delete (*it);
    }
}

void cTemplatePixmap::SetWidth(int width) {
    cString pWidth = cString::sprintf("%d", width);
    parameters->SetWidthManually(*pWidth);
}

void cTemplatePixmap::SetHeight(int height) {
    cString pHeight = cString::sprintf("%d", height);
    parameters->SetHeightManually(*pHeight);
}

void cTemplatePixmap::SetX(int x) {
    parameters->SetXManually(x);
}

void cTemplatePixmap::SetY(int y) {
    parameters->SetYManually(y);
}

void cTemplatePixmap::SetWidthPercent(double width) {
    int absWidth = containerWidth * width;
    cString pWidth = cString::sprintf("%d", absWidth);
    parameters->SetWidthManually(*pWidth);    
}

void cTemplatePixmap::SetHeightPercent(double height) {
    int absHeight = containerHeight * height;
    cString pHeight = cString::sprintf("%d", absHeight);
    parameters->SetHeightManually(*pHeight);    
}

void cTemplatePixmap::SetXPercent(double x) {
    int absX = containerX + containerWidth * x;
    parameters->SetXManually(absX);
}

void cTemplatePixmap::SetYPercent(double y) {
    int absY = containerY + containerHeight * y;
    parameters->SetYManually(absY);
}

void cTemplatePixmap::SetParameter(eParamType type, string value) {
    parameters->SetParameter(type, value);
}


void cTemplatePixmap::ClearDynamicParameters(void) {
    parameters->ClearDynamicParameters();
}

void cTemplatePixmap::ParseDynamicParameters(map <string,string> *stringTokens, map <string,int> *intTokens, bool initFuncs) {
    parameters->ClearDynamicParameters();
    parameters->SetIntTokens(intTokens); 
    parameters->SetStringTokens(stringTokens); 
    parameters->ParseParameters();
    parameters->UnsetIntTokens();
    parameters->UnsetStringTokens();
    
    if (!DoExecute()) {
        parameters->ClearDynamicParameters();
        return;
    }

    if (!initFuncs || !Ready())
        return;

    int x = parameters->GetNumericParameter(ptX);
    int y = parameters->GetNumericParameter(ptY);
    int width = parameters->GetNumericParameter(ptWidth);
    int height = parameters->GetNumericParameter(ptHeight);

    for (vector<cTemplateFunction*>::iterator func = functions.begin(); func != functions.end(); func++) {
        (*func)->SetContainer(x, y, width, height);
        (*func)->ReCalculateParameters();
        (*func)->CompleteParameters();
        if ((*func)->GetType() == ftLoop) {
            cTemplateLoopFunction *loopFunc = dynamic_cast<cTemplateLoopFunction*>(*func);
            if (!loopFunc->Ready()) {
                loopFunc->CalculateParameters();
                loopFunc->SetIntTokens(intTokens);
                loopFunc->ParseParameters();
                loopFunc->UnsetIntTokens();
            }
            loopFunc->CalculateLoopFuncParameters();
        }
    }
}

void cTemplatePixmap::AddFunction(string name, vector<pair<string, string> > &params) {
    eFuncType type = ftNone;

    if (!name.compare("fill")) {
        type = ftFill;
    } else if (!name.compare("drawtext")) {
        type = ftDrawText;
    } else if (!name.compare("drawtextbox")) {
        type = ftDrawTextBox;
    } else if (!name.compare("drawtextvertical")) {
        type = ftDrawTextVertical;
    } else if (!name.compare("drawimage")) {
        type = ftDrawImage;
    } else if (!name.compare("drawrectangle")) {
        type = ftDrawRectangle;
    } else if (!name.compare("drawellipse")) {
        type = ftDrawEllipse;
    } else if (!name.compare("drawslope")) {
        type = ftDrawSlope;
    }

    if (type == ftNone) {
        return;
    }

    cTemplateFunction *f = new cTemplateFunction(type);
    f->SetParameters(params);
    functions.push_back(f);
}

void cTemplatePixmap::AddLoopFunction(cTemplateLoopFunction *lf) {
    functions.push_back(lf);
}


bool cTemplatePixmap::CalculateParameters(void) {
    bool paramsValid = true;
    //Calculate Pixmap Size
    parameters->SetContainer(containerX, containerY, containerWidth, containerHeight);
    parameters->SetGlobals(globals);
    paramsValid = parameters->CalculateParameters();

    int pixWidth = parameters->GetNumericParameter(ptWidth);
    int pixHeight = parameters->GetNumericParameter(ptHeight);

    for (vector<cTemplateFunction*>::iterator func = functions.begin(); func != functions.end(); func++) {
        (*func)->SetGlobals(globals);
        (*func)->SetContainer(0, 0, pixWidth, pixHeight);
        paramsValid = (*func)->CalculateParameters();
        (*func)->CompleteParameters();
        if ((*func)->GetType() == ftLoop) {
            cTemplateLoopFunction *loopFunc = dynamic_cast<cTemplateLoopFunction*>(*func);
            loopFunc->CalculateLoopFuncParameters();
        }
    }

    background = parameters->GetNumericParameter(ptBackground);

    return paramsValid;
}

void cTemplatePixmap::ClearDynamicFunctionParameters(void) {
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        func->ClearDynamicParameters();
    }
}

void cTemplatePixmap::ParseDynamicFunctionParameters(map <string,string> *stringTokens, map <string,int> *intTokens, map < string, vector< map< string, string > > > *loopTokens) {
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    bool completelyParsed = true;
    while(func = GetNextFunction()) {
        func->SetStringTokens(stringTokens);
        func->SetIntTokens(intTokens);
        bool funcCompletelyParsed = func->ParseParameters();
        if (!funcCompletelyParsed)
            completelyParsed = false;
        if (func->Updated())
            func->CompleteParameters();
        func->UnsetIntTokens();
        func->UnsetStringTokens();
    }

    if (completelyParsed) {
        return;
    }

    bool replacedWidth  = ReplaceWidthFunctions();
    bool replacedHeight = ReplaceHeightFunctions(loopTokens);
    bool replacedPosX =   ReplacePosXFunctions();
    bool replacedPosY =   ReplacePosYFunctions();

    if (!replacedWidth && !replacedHeight && !replacedPosX && !replacedPosY)
        return;

    InitFunctionIterator();
    func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely())
            continue;
        func->SetStringTokens(stringTokens);
        func->SetIntTokens(intTokens);
        func->ParseParameters();
        if (func->Updated())
            func->CompleteParameters();
        func->UnsetIntTokens();
        func->UnsetStringTokens();
    }
}

bool cTemplatePixmap::CalculateDrawPortSize(cSize &size, map < string, vector< map< string, string > > > *loopTokens) {
    int pixWidth = parameters->GetNumericParameter(ptWidth);
    int pixHeight = parameters->GetNumericParameter(ptHeight);
    int orientation = parameters->GetNumericParameter(ptOrientation);
    if (orientation < 0)
        orientation = orVertical;
    if (orientation == orHorizontal) {
        //get function which determinates drawport width 
        cTemplateFunction *scrollFunc = GetScrollFunction();
        if (!scrollFunc)
            return false;
        int drawportWidth = scrollFunc->GetWidth(false) + scrollFunc->GetNumericParameter(ptX) + 10;
        if (drawportWidth > pixWidth) {
            size.SetWidth(drawportWidth);
            size.SetHeight(pixHeight);
            return true;
        }
    } else if (orientation == orVertical) {
        //check "last" element height
        InitFunctionIterator();
        cTemplateFunction *f = NULL;
        int drawportHeight = 1;
        while (f = GetNextFunction()) {
            if (f->GetType() == ftLoop) {
                cTemplateLoopFunction *loopFunc = dynamic_cast<cTemplateLoopFunction*>(f);
                //get number of loop tokens
                string loopTokenName = loopFunc->GetParameter(ptName);
                int numLoopTokens = 0;
                map < string, vector< map< string, string > > >::iterator hit = loopTokens->find(loopTokenName);
                if (hit != loopTokens->end()) {
                    vector< map<string,string> > loopToken = hit->second;
                    numLoopTokens = loopToken.size();
                    //parse first loop token element to get correct height
                    vector< map<string,string> >::iterator firstLoopToken = loopToken.begin();
                    loopFunc->ClearDynamicParameters();
                    loopFunc->ParseDynamicParameters(&(*firstLoopToken));
                }
                int orientation = loopFunc->GetNumericParameter(ptOrientation);
                int yFunc = loopFunc->GetNumericParameter(ptY);
                int heightFunc = loopFunc->GetLoopElementsHeight();
                if (loopTokens && orientation == orVertical) {
                    //height is height of loop elements times num loop elements
                    heightFunc = heightFunc * numLoopTokens;
                } else if (loopTokens && orientation == orHorizontal) {
                    int overflow = loopFunc->GetNumericParameter(ptOverflow);
                    if (overflow == otCut) {
                        //do nothing, height is only height of one line
                    } else if (overflow == otWrap) {
                        int widthFunc = loopFunc->GetLoopElementsWidth();
                        if (widthFunc <= 0)
                            continue;
                        int loopWidth = loopFunc->GetNumericParameter(ptWidth);
                        if (loopWidth <= 0)
                            loopWidth = loopFunc->GetContainerWidth();
                        int elementsPerRow = loopWidth / widthFunc;
                        int rest = loopWidth % widthFunc;
                        if (rest > 0)
                            elementsPerRow++;
                        if (elementsPerRow <= 0)
                            continue;
                        int lines = numLoopTokens / elementsPerRow;
                        rest = numLoopTokens % elementsPerRow;
                        if (rest > 0)
                            lines++;
                        heightFunc = heightFunc * lines;  
                    }
                }
                int neededHeight = heightFunc + yFunc;
                if (neededHeight > drawportHeight)
                    drawportHeight = neededHeight;
            } else {
                int yFunc = f->GetNumericParameter(ptY);
                int heightFunc = f->GetHeight();
                int neededHeight = heightFunc + yFunc;
                if (neededHeight > drawportHeight)
                    drawportHeight = neededHeight;
            }
        }
        if (drawportHeight > pixHeight) {
            size.SetWidth(pixWidth);
            size.SetHeight(drawportHeight);
            return true;
        }
    }
    size.SetWidth(0);
    size.SetHeight(0);
    return false;
}

void cTemplatePixmap::SetScrollingTextWidth(void) {
    int orientation = parameters->GetNumericParameter(ptOrientation);
    if (orientation != orHorizontal)
        return;
    int pixWidth = parameters->GetNumericParameter(ptWidth);
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        if (func->GetType() == ftDrawText) {
            int offset = func->GetNumericParameter(ptX);
            func->SetMaxTextWidth(pixWidth - offset);
        }
    }        
}


cTemplateFunction *cTemplatePixmap::GetScrollFunction(void) {
    string scrollElement = parameters->GetParameter(ptScrollElement);
    if (scrollElement.size() == 0)
        return NULL;
    InitFunctionIterator();
    cTemplateFunction *f = NULL;
    while (f = GetNextFunction()) {
        string funcName = f->GetParameter(ptName);
        if (!funcName.compare(scrollElement)) {
            return f;
        }
    }
    return NULL;
}

cRect cTemplatePixmap::GetPixmapSize(void) {
    cRect size;
    size.SetX(GetNumericParameter(ptX));
    size.SetY(GetNumericParameter(ptY));
    size.SetWidth(GetNumericParameter(ptWidth));
    size.SetHeight(GetNumericParameter(ptHeight));
    return size;
}

int cTemplatePixmap::GetNumericParameter(eParamType type) {
    if (!parameters)
        return -1;
    return parameters->GetNumericParameter(type);
}

void cTemplatePixmap::InitFunctionIterator(void) {
    funcIt = functions.begin();
}

cTemplateFunction *cTemplatePixmap::GetNextFunction(void) {
    if (funcIt == functions.end())
        return NULL;
    cTemplateFunction *func = *funcIt;
    funcIt++;
    return func;
}

bool cTemplatePixmap::Ready(void) {
    int myX = parameters->GetNumericParameter(ptX);
    if (myX < 0)
        return false;
    int myY = parameters->GetNumericParameter(ptY);
    if (myY < 0)
        return false;
    int myWidth = parameters->GetNumericParameter(ptWidth);
    if (myWidth < 1)
        return false;
    int myHeight = parameters->GetNumericParameter(ptHeight);
    if (myHeight < 1)
        return false;
    return true;
}

bool cTemplatePixmap::ParameterSet(eParamType type) {
    string value = parameters->GetParameter(type);
    if (value.size() > 0)
        return true;
    return false;
}

cTemplateFunction *cTemplatePixmap::GetFunction(string name) {
    for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
        string funcName = (*it)->GetParameter(ptName);
        if (!name.compare(funcName))
            return *it;
    }
    return NULL;
}

bool cTemplatePixmap::ReplaceWidthFunctions(void) {
    bool replaced = false;
    bool found = false;
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely()) {
            continue;
        }
        multimap<eParamType,string> widths;
        func->GetNeededWidths(&widths);
        for (map<eParamType, string>::iterator names = widths.begin(); names !=widths.end(); names++) {
            eParamType type = names->first;
            string label = names->second;
            int funcWidth = 0;
            for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
                cTemplateFunction *myFunc = *it;
                string myFuncName = myFunc->GetParameter(ptName); 
                if (!myFuncName.compare(label)) {
                    found = true;
                    funcWidth = myFunc->GetWidth();
                    func->SetWidth(type, label, funcWidth);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                    break;
                }
            }
            if (!found && pixContainer) {
                cTemplateFunction *myFunc = pixContainer->GetFunction(label);
                if (myFunc) {
                    funcWidth = myFunc->GetWidth();
                    func->SetWidth(type, label, funcWidth);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                    break;
                }
            }
        }
    }
    return replaced;
}

bool cTemplatePixmap::ReplaceHeightFunctions(map < string, vector< map< string, string > > > *loopTokens) {
    bool replaced = false;
    bool found = false;
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely()) {
            continue;
        }
        multimap<eParamType,string> heights;
        func->GetNeededHeights(&heights);
        for (map<eParamType, string>::iterator names = heights.begin(); names !=heights.end(); names++) {
            eParamType type = names->first;
            string label = names->second;
            int funcHeight = 0;
            for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
                cTemplateFunction *myFunc = *it;
                string myFuncName = myFunc->GetParameter(ptName); 
                if (!myFuncName.compare(label)) {
                    found = true;
                    funcHeight = myFunc->GetHeight(loopTokens);
                    func->SetHeight(type, label, funcHeight);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                    break;
                }
            }
            if (!found && pixContainer) {
                cTemplateFunction *myFunc = pixContainer->GetFunction(label);
                if (myFunc) {
                    funcHeight = myFunc->GetHeight(loopTokens);
                    func->SetHeight(type, label, funcHeight);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                    break;
                }
            }
        }
    }
    return replaced;
}

bool cTemplatePixmap::ReplacePosXFunctions(void) {
    bool replaced = false;
    bool found = false;
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely()) {
            continue;
        }
        multimap<eParamType,string> posXs;
        func->GetNeededPosX(&posXs);
        for (map<eParamType, string>::iterator names = posXs.begin(); names !=posXs.end(); names++) {
            eParamType type = names->first;
            string label = names->second;
            int funcX = 0;
            for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
                cTemplateFunction *myFunc = *it;
                string myFuncName = myFunc->GetParameter(ptName); 
                if (!myFuncName.compare(label)) {
                    found = true;
                    funcX = myFunc->GetNumericParameter(ptX);
                    if (funcX > -1) {
                        func->SetX(type, label, funcX);
                        if (func->Updated()) {
                            func->CompleteParameters();
                        } else {
                            replaced = true;
                        }
                    }
                    break;
                }
            }
            if (!found && pixContainer) {
                cTemplateFunction *myFunc = pixContainer->GetFunction(label);
                if (myFunc) {
                    funcX = myFunc->GetNumericParameter(ptX);
                    if (funcX > -1) {
                        func->SetX(type, label, funcX);
                        if (func->Updated()) {
                            func->CompleteParameters();
                        } else {
                            replaced = true;
                        }
                    }
                    break;
                }
            }
        }
    }
    return replaced;
}

bool cTemplatePixmap::ReplacePosYFunctions(void) {
    bool replaced = false;
    bool found = false;
    InitFunctionIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely()) {
            continue;
        }
        multimap<eParamType,string> posYs;
        func->GetNeededPosY(&posYs);
        for (map<eParamType, string>::iterator names = posYs.begin(); names !=posYs.end(); names++) {
            eParamType type = names->first;
            string label = names->second;
            int funcY = 0;
            for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
                cTemplateFunction *myFunc = *it;
                string myFuncName = myFunc->GetParameter(ptName);
                if (!myFuncName.compare(label)) {
                    found = true;
                    funcY = myFunc->GetNumericParameter(ptY);
                    if (funcY > -1) {
                        func->SetY(type, label, funcY);
                        if (func->Updated()) {
                            func->CompleteParameters();
                        } else {
                            replaced = true;
                        }
                    }
                    break;
                }
            }
            if (!found && pixContainer) {
                cTemplateFunction *myFunc = pixContainer->GetFunction(label);
                if (myFunc) {
                    funcY = myFunc->GetNumericParameter(ptY);
                    if (funcY > -1) {
                        func->SetY(type, label, funcY);
                        if (func->Updated()) {
                            func->CompleteParameters();
                        } else {
                            replaced = true;
                        }
                    }
                    break;
                }
            }
        }
    }
    return replaced;
}

void cTemplatePixmap::Debug(void) {
    esyslog("skindesigner: pixmap container size x: %d, y: %d, width: %d, height %d", containerX, containerY, containerWidth, containerHeight);
    parameters->Debug();
    for (vector<cTemplateFunction*>::iterator it = functions.begin(); it != functions.end(); it++) {
        (*it)->Debug();
    }
}

// --- cTemplatePixmapContainer -------------------------------------------------------------

cTemplatePixmapContainer::cTemplatePixmapContainer(void) {
    isContainer = true;
}

cTemplatePixmapContainer::~cTemplatePixmapContainer() {
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        delete (*it);
    }
}

void cTemplatePixmapContainer::SetGlobals(cGlobals *globals) {
    this->globals = globals;
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        (*it)->SetGlobals(globals);
    }    
}

void cTemplatePixmapContainer::SetWidth(int width) {
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        (*it)->SetWidth(width);
    }
}

void cTemplatePixmapContainer::SetHeight(int height) {
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        (*it)->SetHeight(height);
    }
}

void cTemplatePixmapContainer::AddPixmap(cTemplatePixmap *pix) {
    //setting default parameters of container if parameter is not set in area
    string containerDefaultX = parameters->GetParameter(ptX);
    string containerDefaultY = parameters->GetParameter(ptY);
    string containerDefaultWidth = parameters->GetParameter(ptWidth);
    string containerDefaultHeight = parameters->GetParameter(ptHeight);
    if (containerDefaultX.size() > 0 && !pix->ParameterSet(ptX))
        pix->SetParameter(ptX, containerDefaultX);
    if (containerDefaultY.size() > 0 && !pix->ParameterSet(ptY))
        pix->SetParameter(ptY, containerDefaultY);
    if (containerDefaultWidth.size() > 0 && !pix->ParameterSet(ptWidth))
        pix->SetParameter(ptWidth, containerDefaultWidth);
    if (containerDefaultHeight.size() > 0 && !pix->ParameterSet(ptHeight))
        pix->SetParameter(ptHeight, containerDefaultHeight);

    pix->SetPixmapContainer(this);
    pixmaps.push_back(pix); 
}

bool cTemplatePixmapContainer::CalculateParameters(void) {
    bool paramsValid = true;
    //Calculate Pixmap Size
    parameters->SetContainer(containerX, containerY, containerWidth, containerHeight);
    parameters->SetGlobals(globals);
    paramsValid = parameters->CalculateParameters();

    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        (*it)->SetContainer(containerX, containerY, containerWidth, containerHeight);
        (*it)->SetGlobals(globals);
        paramsValid = (*it)->CalculateParameters() && paramsValid;
    }

    return paramsValid;
}

void cTemplatePixmapContainer::ParseDynamicParameters(map <string,string> *stringTokens, map <string,int> *intTokens) {
    parameters->ClearDynamicParameters();
    parameters->SetIntTokens(intTokens); 
    parameters->SetStringTokens(stringTokens); 
    parameters->ParseParameters();
    parameters->UnsetIntTokens();
    parameters->UnsetStringTokens();
}

void cTemplatePixmapContainer::InitIterator(void) {
    pixmapIterator = pixmaps.begin();
}

cTemplatePixmap *cTemplatePixmapContainer::GetNextPixmap(void) {
    if (pixmapIterator == pixmaps.end())
        return NULL;
    cTemplatePixmap *pix = *pixmapIterator;
    pixmapIterator++;
    return pix;
}

cTemplateFunction *cTemplatePixmapContainer::GetFunction(string name) {
    cTemplateFunction *hit = NULL;
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        hit  = (*it)->GetFunction(name);
        if (hit)
            return hit;
    }
    return NULL;
}


void cTemplatePixmapContainer::Debug(void) {
    esyslog("skindesigner: pixmapcontainer");
    parameters->Debug();
    for (vector<cTemplatePixmap*>::iterator it = pixmaps.begin(); it != pixmaps.end(); it++) {
        (*it)->Debug();
    }
}

