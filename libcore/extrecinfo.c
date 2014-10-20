#include "../config.h"
#include "helpers.h"
#include "extrecinfo.h"

cExtRecInfo::cExtRecInfo(const char *xml) {
	this->xml = xml;
}

cExtRecInfo::~cExtRecInfo(void) {
	
}

bool cExtRecInfo::Parse(void) {
	//read media info
	string mediaInfoXml;
	StripXmlTag(xml, mediaInfoXml, "mediainfo");
	if (mediaInfoXml.size() == 0) {
		return false;
	}
	StripXmlTag(mediaInfoXml, resWidth, "res_width");
	StripXmlTag(mediaInfoXml, resHeight, "res_height");
	resString = GetScreenResolutionString(resWidth, resHeight, &isHD);
	StripXmlTag(mediaInfoXml, aspectratio, "aspectratio");
	isWideScreen = !aspectratio.compare("16:9");
	StripXmlTag(mediaInfoXml, codec, "codec");
	StripXmlTag(mediaInfoXml, format, "format");
 	StripXmlTag(mediaInfoXml, framerate, "framerate");
	StripXmlTag(mediaInfoXml, interlace, "interlace");

	size_t found = 0;
	isDolby = false;
	do {
		string track;
		found = StripXmlTag(mediaInfoXml, track, "track", found);
		if (found == string::npos)
			break;
		tAudioTrack sTrack;
		StripXmlTag(track, sTrack.codec, "codec");
		StripXmlTag(track, sTrack.bitrate, "bitrate");
		StripXmlTag(track, sTrack.language, "language");
		if (!sTrack.codec.compare("AC-3"))
			isDolby = true;
		tracks.push_back(sTrack);
	} while (found != string::npos);

	return true;
}

//get content of <tag> ... </tag> inside xml
size_t cExtRecInfo::StripXmlTag(string &xmlstring, string &content, const char *tag, int start) {
    // set the search strings
    stringstream strStart, strStop;
    strStart << "<" << tag << ">";
    strStop << "</" << tag << ">";
    // find the strings
    size_t locStart = xmlstring.find(strStart.str(), start);
    size_t locStop  = xmlstring.find(strStop.str(), start);
    if (locStart == string::npos || locStop == string::npos)
        return string::npos;
    // extract relevant text
    int pos = locStart + strStart.str().size();
    int len = locStop - pos;
    
    content = (len < 0) ? "" : xmlstring.substr(pos, len);
    return locStop + strStop.str().size();
}

size_t cExtRecInfo::StripXmlTag(string &xmlstring, int &content, const char *tag, int start) {
    // set the search strings
    stringstream strStart, strStop;
    strStart << "<" << tag << ">";
    strStop << "</" << tag << ">";
    // find the strings
    size_t locStart = xmlstring.find(strStart.str(), start);
    size_t locStop  = xmlstring.find(strStop.str(), start);
    if (locStart == string::npos || locStop == string::npos)
        return string::npos;
    // extract relevant text
    int pos = locStart + strStart.str().size();
    int len = locStop - pos;
    
    string value = (len < 0) ? "" : xmlstring.substr(pos, len);
    content = atoi(value.c_str());
    return locStop + strStop.str().size();
}

void cExtRecInfo::Debug(void) {
	dsyslog("skindesigner: extRecInfo xml: %s", xml.c_str());
	dsyslog("skindesigner: : res_width %d, res_height %d, res %s, aspectratio %s, codec %s, format %s, framerate %s, interlace %s, hd %s, widescreen %s", 
			resWidth, resHeight, resString.c_str(), aspectratio.c_str(), codec.c_str(), format.c_str(), framerate.c_str(), interlace.c_str(),
			isHD ? "true": "false", isWideScreen ? "true" : "false");
	int numTrack = 1;
	for (vector<tAudioTrack>::iterator it = tracks.begin(); it != tracks.end(); it++) {
		dsyslog("skindesigner: audio track %d, codec %s, bitrate %s, language: %s", numTrack++, (*it).codec.c_str(), (*it).bitrate.c_str(), (*it).language.c_str());
	}
}
