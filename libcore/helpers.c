#include <string>
#include <sstream>
#include <vector>
#include "helpers.h"
#include <vdr/skins.h>

cPlugin *GetScraperPlugin(void) {
    static cPlugin *pScraper = cPluginManager::GetPlugin("scraper2vdr");
    if( !pScraper ) // if it doesn't exit, try tvscraper
        pScraper = cPluginManager::GetPlugin("tvscraper");
    return pScraper;
}

cSize ScaleToFit(int widthMax, int heightMax, int widthOriginal, int heightOriginal) {
    int width = 1;
    int height = 1;

    if ((widthMax == 0)||(heightMax==0)||(widthOriginal==0)||(heightOriginal==0))
        return cSize(width, height);

    if ((widthOriginal <= widthMax) && (heightOriginal <= heightMax)) {
        width = widthOriginal;
        height = heightOriginal;
    } else if ((widthOriginal > widthMax) && (heightOriginal <= heightMax)) {
        width = widthMax;
        height = (double)width/(double)widthOriginal * heightOriginal;
    } else if ((widthOriginal <= widthMax) && (heightOriginal > heightMax)) {
        height = heightMax;
        width = (double)height/(double)heightOriginal * widthOriginal;
    } else {
        width = widthMax;
        height = (double)width/(double)widthOriginal * heightOriginal;
        if (height > heightMax) {
            height = heightMax;
            width = (double)height/(double)heightOriginal * widthOriginal;
        }
    }
    return cSize(width, height);
}

int Minimum(int a, int b, int c, int d, int e, int f) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    if (d < min) min = d;
    if (e < min) min = e;
    if (f < min) min = f;
    return min;
}

string CutText(string &text, int width, string fontName, int fontSize) {
    if (width <= fontManager->Font(fontName, fontSize)->Size())
        return text.c_str();
    cTextWrapper twText;
    twText.Set(text.c_str(), fontManager->Font(fontName, fontSize), width);
    string cuttedTextNative = twText.GetLine(0);
    stringstream sstrText;
    sstrText << cuttedTextNative << "...";
    string cuttedText = sstrText.str();
    int actWidth = fontManager->Width(fontName, fontSize, cuttedText.c_str());
    if (actWidth > width) {
        int overlap = actWidth - width;
        int charWidth = fontManager->Width(fontName, fontSize, ".");
        if (charWidth == 0)
            charWidth = 1;
        int cutChars = overlap / charWidth;
        if (cutChars > 0) {
            cuttedTextNative = cuttedTextNative.substr(0, cuttedTextNative.length() - cutChars);
            stringstream sstrText2;
            sstrText2 << cuttedTextNative << "...";
            cuttedText = sstrText2.str();
        }
    }
    return cuttedText;    
}


string StrToLowerCase(string str) {
    string lowerCase = str;
    const int length = lowerCase.length();
    for(int i=0; i < length; ++i) {
        lowerCase[i] = std::tolower(lowerCase[i]);
    }
    return lowerCase;
}

bool isNumber(const string& s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool FileExists(const string &path, const string &name, const string &ext) {
    stringstream fileName;
    fileName << path << name << "." << ext;
    struct stat buffer;
    return (stat (fileName.str().c_str(), &buffer) == 0); 
}

bool FolderExists(const string &path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode);
}

bool FirstFileInFolder(string &path, string &extension, string &fileName) {
    DIR *folder = NULL;
    struct dirent *file;
    folder = opendir(path.c_str());
    if (!folder)
        return false;
    while (file = readdir(folder)) {
        if (endswith(file->d_name, extension.c_str())) {
            string currentFileName = file->d_name;
            int strlength = currentFileName.size();
            if (strlength < 8)
                continue;
            fileName = currentFileName;
            return true;
        }
    }
    return false;
}

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string>& splitstring::split(char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}

cStopWatch::cStopWatch(void) {
    start = cTimeMs::Now();
    last = start;
}

void cStopWatch::Report(const char* message) {
    dsyslog("skindesigner: %s - needed %d ms", message, (int)(cTimeMs::Now() - last));
    last = cTimeMs::Now();
}

void cStopWatch::Stop(const char* message) {
    dsyslog("skindesigner: %s - needed %d ms", message, (int)(cTimeMs::Now() - start));
}
