#ifndef __SKINSETUPPARAMETER_H
#define __SKINSETUPPARAMETER_H

#include <string>
#include <vdr/plugin.h>

using namespace std;

enum eSetupParameterType {
    sptInt,
    sptBool,
    sptUnknown
};

// --- cSkinSetupParameter -----------------------------------------------------------

class cSkinSetupParameter {
private:
public:
    cSkinSetupParameter(void);
    virtual ~cSkinSetupParameter(void) {};
	eSetupParameterType type;
	string name;
	string displayText;
	int min;
	int max;
	int value; 
    void Debug(void);
};

#endif //__SKINSETUPPARAMETER_H