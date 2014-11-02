#ifndef __TEMPLATEPARAMETER_H
#define __TEMPLATEPARAMETER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "globals.h"

using namespace std;

enum eAlign {
    alLeft,
    alCenter,
    alRight,
    alTop,
    alBottom
};

enum eScrollMode {
    smNone,
    smCarriageReturn,
    smForthAndBack
};

enum eScrollSpeed {
    ssNone,
    ssSlow,
    ssMedium,
    ssFast
};

enum eOrientation {
    orNone,
    orHorizontal,
    orVertical,
    orAbsolute
};

// --- cNumericParameter -------------------------------------------------------------

class cNumericParameter {
private:
    cGlobals *globals;
    string value;
    bool isValid;
    int width;
    int height;
    int columnWidth;
    int rowHeight;
    bool hor;
    int defaultValue;
    bool IsNumber(const string& s);
    bool CheckPercentValue(int &val);
    bool CheckExpression(int &val, string &parsedVal);
    bool ValidNumericExpression(string &parsedValue);
    int EvaluateTheExpression(char* expr);
    double ParseAtom(char*& expr);
    double ParseFactors(char*& expr);
    double ParseSummands(char*& expr);
public:
    cNumericParameter(string value);
    virtual ~cNumericParameter(void);
    void SetGlobals(cGlobals *globals) { this->globals = globals; };
    void SetAreaSize(int w, int h);
    void SetLoopContainer(int columnWidth, int rowHeight) { this->columnWidth = columnWidth; this->rowHeight = rowHeight; };
    void SetDefault(int def) { defaultValue = def; };
    void SetHorizontal(void) { hor = true; };
    void SetVertical(void) { hor = false; };
    int Parse(string &parsedValue);
    bool Valid(void) { return isValid; };
};

// --- cTextToken -------------------------------------------------------------

enum eTextTokenType {
    ttConstString,
    ttToken,
    ttConditionalToken,
    ttPrintfToken
};

class cTextToken {
public:
    eTextTokenType type;
    string value;
    vector<string> parameters;
    vector<cTextToken> subTokens;
};

// --- cConditionalParameter -------------------------------------------------------------

enum eCondParameterType {
    cpAnd,
    cpOr,
    cpNone
};

enum eCondType {
    ctLower,
    ctGreater,
    ctEquals,
    ctBool,
    ctStringSet,
    ctNone
};

struct sCondition {
    string tokenName;
    bool isNegated;
    eCondType type;
    int compareValue;
};

class cConditionalParameter {
private:
    cGlobals *globals;
    bool isTrue;
    string value;
    eCondParameterType type;
    vector<sCondition> conditions;
    void TokenizeValue(string sep);
    void InsertCondition(string cond);
    int EvaluateParameter(string token, map < string, int > *intTokens, map < string, string > *stringTokens);
public:
    cConditionalParameter(cGlobals *globals, string value);
    virtual ~cConditionalParameter(void);
    void Tokenize(void);
    bool Evaluate(map < string, int > *intTokens, map < string, string > *stringTokens);
    bool IsTrue(void) { return isTrue; };
    void Debug(void);
};
#endif //__TEMPLATEPARAMETER_H