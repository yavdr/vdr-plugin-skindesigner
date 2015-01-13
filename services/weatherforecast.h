#ifndef __WEATHERFORECASTSERVICES_H
#define __WEATHERFORECASTSERVICES_H

#include <string>

class cServiceCurrentWeather {
public:
	cServiceCurrentWeather(void) {
	    timeStamp = "";
	    temperature = "";
	    apparentTemperature = "";
	    summary = "";
	    icon = "";
	    precipitationIntensity = "";
	    precipitationProbability = 0;
	    precipitationType = "";
	    humidity = 0;
	    windSpeed = "";
	    windBearing = 0;
	    windBearingString = "";
	    visibility = "";
	    cloudCover = 0;
	    pressure = "";
	    ozone = "";
	};
    std::string timeStamp;
    std::string temperature;
    std::string apparentTemperature;
    std::string summary;
    std::string icon;
    std::string precipitationIntensity;
    int precipitationProbability;
    std::string precipitationType;
    int humidity;
    std::string windSpeed;
    int windBearing;
    std::string windBearingString;
    std::string visibility;
    int cloudCover;
    std::string pressure;
    std::string ozone;
};

#endif //__WEATHERFORECASTSERVICES_H

