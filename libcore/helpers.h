#ifndef __HELPERS_H
#define __HELPERS_H

#include <vdr/osd.h>
#include <vdr/plugin.h>
#include "../config.h"

cPlugin *GetScraperPlugin(void);

cSize ScaleToFit(int widthMax, int heightMax, int widthOriginal, int heightOriginal);
int Minimum(int a, int b, int c, int d, int e, int f);
std::string CutText(string &text, int width, string fontName, int fontSize);
std::string StrToLowerCase(string str);
bool isNumber(const string& s);
bool FileExists(const string &path, const string &name, const string &ext);
bool FolderExists(const string &path);
bool FirstFileInFolder(string &path, string &extension, string &fileName);

class splitstring : public std::string {
    std::vector<std::string> flds;
public:
    splitstring(const char *s) : std::string(s) { };
    std::vector<std::string>& split(char delim, int rep=0);
};

class cStopWatch {
private:
	uint64_t start;
	uint64_t last;
public:
	cStopWatch(void);
	~cStopWatch(void) {};
	void Report(const char* message);
	void Stop(const char* message);
};
#endif // __HELPERS_H
