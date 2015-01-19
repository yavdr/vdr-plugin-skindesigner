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
	if (type == sptBool)
		dsyslog("skindesigner: type bool");
	else if (type == sptInt)
		dsyslog("skindesigner: type integer");
	else
		dsyslog("skindesigner: type UNKNOWN");
	dsyslog("skindesigner: name %s", name.c_str());
	dsyslog("skindesigner: displayText %s", displayText.c_str());
	if (type == sptInt)
		dsyslog("skindesigner: min %d, max %d", min, max);
	dsyslog("skindesigner: Value %d", value);
}