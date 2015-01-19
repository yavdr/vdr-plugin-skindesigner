#include "skinsetupparameter.h"

cSkinSetupParameter::cSkinSetupParameter(void) {
	type = sptUnknown;
	name = "";
	displayText = "";
	min = 0;
	max = 1000;
	value = 0; 
}

void cSkinSetupParameter::Debug(void) {
	string sType = "unknown";
	if (type == sptBool)
		sType = "bool";
	else if (type == sptInt)
		sType = "int";
	dsyslog("skindesigner: name \"%s\", type %s, displayText \"%s\", Value %d", name.c_str(), sType.c_str(), displayText.c_str(), value);
	if (type == sptInt)
		dsyslog("skindesigner: min %d, max %d", min, max);
}