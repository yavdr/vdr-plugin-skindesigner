#ifndef __EXTRECINFO_H
#define __EXTRECINFO_H

#include <vdr/recording.h>

struct tAudioTrack {
  string codec;
  string bitrate;
  string language;
};

class cExtRecInfo {
private:
  string xml;
  size_t StripXmlTag(string &xmlstring, string &content, const char *tag, int start = 0);
  size_t StripXmlTag(string &xmlstring, int &content, const char *tag, int start = 0);
public:
  cExtRecInfo(const char *xml);
  ~cExtRecInfo(void);
  bool Parse(void);
  void Debug(void);
  int resWidth;
  int resHeight;
  string resString;
  bool isHD;
  string aspectratio;
  bool isWideScreen;
  string codec;
  string format;
  string framerate;
  string interlace;
  bool isDolby;
  vector< tAudioTrack > tracks; 
};

#endif // __EXTRECINFO_H