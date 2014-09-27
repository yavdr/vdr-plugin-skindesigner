#ifndef __TEMPLATELOOPFUNCTION_H
#define __TEMPLATELOOPFUNCTION_H

#include "templatefunction.h"

using namespace std;

// --- cTemplateLoopFunction -------------------------------------------------------------

class cTemplateLoopFunction : public cTemplateFunction {
private:
    vector<cTemplateFunction*> functions;
    vector<cTemplateFunction*>::iterator funcIt;
    void ReplaceWidthFunctions(void);
    void ReplaceHeightFunctions(void);
public:
    cTemplateLoopFunction(void);
    virtual ~cTemplateLoopFunction(void);
    void AddFunction(string name, vector<pair<string, string> > &params);
    void CalculateLoopFuncParameters(void);
    void InitIterator(void);
    cTemplateFunction *GetNextFunction(void);
    void ClearDynamicParameters(void);
    void ParseDynamicParameters(map <string,string> *tokens);
    int GetLoopElementsWidth(void);
    int GetLoopElementsHeight(void);
    int GetContainerWidth(void) { return containerWidth; };
    int GetContainerHeight(void) { return containerHeight; };
    bool Ready(void);
    void Debug(void);
};

#endif //__TEMPLATELOOPFUNCTION_H
