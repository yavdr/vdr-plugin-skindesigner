#include "templateloopfunction.h"
#include "../libcore/helpers.h"

using namespace std;

// --- cTemplateFunction -------------------------------------------------------------

cTemplateLoopFunction::cTemplateLoopFunction(void) : cTemplateFunction(ftLoop) { 
}

cTemplateLoopFunction::~cTemplateLoopFunction(void) {
}

void cTemplateLoopFunction::InitIterator(void) {
    funcIt = functions.begin();
}

void cTemplateLoopFunction::AddFunction(string name, vector<pair<string, string> > &params) {
    eFuncType type = ftNone;

    if (!name.compare("drawtext")) {
        type = ftDrawText;
    } else if (!name.compare("drawtextbox")) {
        type = ftDrawTextBox;
    } else if (!name.compare("drawimage")) {
        type = ftDrawImage;
    } else if (!name.compare("drawrectangle")) {
        type = ftDrawRectangle;
    } else if (!name.compare("drawellipse")) {
        type = ftDrawEllipse;
    }

    if (type == ftNone) {
        return;
    }

    cTemplateFunction *f = new cTemplateFunction(type);
    f->SetParameters(params);
    functions.push_back(f);
}

void cTemplateLoopFunction::CalculateLoopFuncParameters(void) {
    int columnWidth = GetNumericParameter(ptColumnWidth);
    int rowHeight = GetNumericParameter(ptRowHeight);
    for (vector<cTemplateFunction*>::iterator func = functions.begin(); func != functions.end(); func++) {
        (*func)->SetGlobals(globals);
        (*func)->SetContainer(0, 0, containerWidth, containerHeight);
        (*func)->SetLoopContainer(columnWidth, rowHeight);
        (*func)->CalculateParameters();
        (*func)->CompleteParameters();
    }
}

cTemplateFunction *cTemplateLoopFunction::GetNextFunction(void) {
    if (funcIt == functions.end())
        return NULL;
    cTemplateFunction *func = *funcIt;
    funcIt++;
    return func;
}

void cTemplateLoopFunction::ClearDynamicParameters(void) {
    InitIterator();
    cTemplateFunction *func = NULL;
    while(func = GetNextFunction()) {
        func->ClearDynamicParameters();
    }
}

void cTemplateLoopFunction::ParseDynamicParameters(map <string,string> *tokens) {
    if (!tokens)
        return;
    InitIterator();
    cTemplateFunction *func = NULL;
    
    map <string,int> intTokens;
    for (map <string,string>::iterator it = tokens->begin(); it != tokens->end(); it++) {
        if (isNumber(it->second))
            intTokens.insert(pair<string, int>(it->first, atoi((it->second).c_str())));
    } 

    bool completelyParsed = true;
    while(func = GetNextFunction()) {
        func->SetStringTokens(tokens);
        func->SetIntTokens(&intTokens);
        bool funcCompletelyParsed = func->ParseParameters();
        if (!funcCompletelyParsed)
            completelyParsed = false;
        if (func->Updated())
            func->CompleteParameters();
        func->UnsetStringTokens();
        func->UnsetIntTokens();
    }
    if (completelyParsed) {
        return;
    }

    bool replacedWidth = ReplaceWidthFunctions();
    bool replacedHeight = ReplaceHeightFunctions();

    if (!replacedWidth && !replacedHeight)
        return;

    InitIterator();
    func = NULL;
    while(func = GetNextFunction()) {
        if (func->ParsedCompletely())
            continue;
        func->SetStringTokens(tokens);
        func->SetIntTokens(&intTokens);
        func->ParseParameters();
        if (func->Updated())
            func->CompleteParameters();
        func->UnsetIntTokens();
        func->UnsetStringTokens();
    }
}

int cTemplateLoopFunction::GetLoopElementsWidth(void) {
    int cW = GetNumericParameter(ptColumnWidth);
    if (cW > 0) {
        return cW;
    }
    InitIterator();
    cTemplateFunction *func = NULL;
    int maxWidth = 1;
    while(func = GetNextFunction()) {
        int funcWidth = func->GetWidth(true);
        if (funcWidth > maxWidth)
            maxWidth = funcWidth;
    }
    return maxWidth;
}

int cTemplateLoopFunction::GetLoopElementsHeight(void) {
    int rH = GetNumericParameter(ptRowHeight);
    if (rH > 0)
        return rH;
    InitIterator();
    cTemplateFunction *func = NULL;
    int maxHeight = 1;
    while(func = GetNextFunction()) {
        int funcY = func->GetNumericParameter(ptY);
        int funcHeight = func->GetHeight();
        int totalHeight = funcY + funcHeight;
        if (totalHeight > maxHeight)
            maxHeight = totalHeight;
    }
    return maxHeight;
}

int cTemplateLoopFunction::CalculateHeight(map < string, vector< map< string, string > > > *loopTokens) {
    if (!loopTokens) {
        return 0;
    }
    int rowHeight = GetLoopElementsHeight();
    string loopName = GetParameter(ptName);
    map < string, vector< map< string, string > > >::iterator hit = loopTokens->find(loopName);
    if (hit == loopTokens->end())
        return 0;
    vector< map< string, string > > toks = hit->second;
    int numElements = toks.size();

    return numElements * rowHeight;
}

bool cTemplateLoopFunction::ReplaceWidthFunctions(void) {
    bool replaced = false;
    InitIterator();
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
                    funcWidth = myFunc->GetWidth();
                    func->SetWidth(type, label, funcWidth);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                }
            }
        }
    }
    return replaced;
}

bool cTemplateLoopFunction::ReplaceHeightFunctions(void) {
    bool replaced = false;
    InitIterator();
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
                    funcHeight = myFunc->GetHeight();
                    func->SetHeight(type, label, funcHeight);
                    if (func->Updated()) {
                        func->CompleteParameters();
                    } else {
                        replaced = true;
                    }
                }
            }
        }
    }
    return replaced;
}

bool cTemplateLoopFunction::Ready(void) {
    bool isReady = true;
    map< eParamType, string >::iterator hit = numericDynamicParameters.find(ptColumnWidth);
    if (hit != numericDynamicParameters.end())
        isReady = false;
    hit = numericDynamicParameters.find(ptRowHeight);
    if (hit != numericDynamicParameters.end())
        isReady = false;
    return isReady;
}

void cTemplateLoopFunction::Debug(void) {
    cTemplateFunction::Debug();
    esyslog("skindesigner: functions to be looped:");
    for (vector<cTemplateFunction*>::iterator func = functions.begin(); func != functions.end(); func++) {
        (*func)->Debug();
    }
}