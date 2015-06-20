#ifndef __FONTMANAGER_H
#define __FONTMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <vdr/skins.h>

#include "../libtemplate/template.h"

using namespace std;

class cFontManager {
private:
    static cMutex mutex;
    map < string, map< int, cFont* > > fonts;
    cFont *CreateFont(string name, int size);
    void InsertFont(string name, int size);
    bool FontExists(string name, int size);
    cFont *GetFont(string name, int size);
    int GetFontHeight(const char *name, int height, int charWidth = 0);
public:
    cFontManager();
    ~cFontManager();
    void Lock(void) { mutex.Lock(); };
    void Unlock(void) { mutex.Unlock(); };
    void CacheFonts(cTemplate *tpl);
    void DeleteFonts(void);
    int Width(string fontName, int fontSize, const char *text);
    int Height(string fontName, int fontSize);
    cFont *Font(string fontName, int fontSize);
    cFont *FontUncached(string fontName, int fontSize);
    void Debug(void);
    void ListAvailableFonts(void);
    bool FontInstalled(string fontName);
};

#endif //__FONTMANAGER_H