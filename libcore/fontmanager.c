#include "fontmanager.h"
#include "../config.h"
#include <ft2build.h>   
#include FT_FREETYPE_H

using namespace std;

cMutex cFontManager::mutex;

cFontManager::cFontManager() {
}

cFontManager::~cFontManager() {
    DeleteFonts();
}

void cFontManager::CacheFonts(cTemplate *tpl) {
    cMutexLock MutexLock(&mutex);

    vector< pair<string, int> > usedFonts = tpl->GetUsedFonts();

    cStringList availableFonts;
    cFont::GetAvailableFontNames(&availableFonts);

    for (vector< pair<string, int> >::iterator ft = usedFonts.begin(); ft != usedFonts.end(); ft++) {
        string fontName = ft->first;
        int fontSize = ft->second;
        if (fontSize < 1) {
            continue;
        }

        int fontAvailable = availableFonts.Find(fontName.c_str());
        if (fontAvailable == -1) {
            esyslog("skindesigner: font %s not available, skipping", fontName.c_str());
            continue;
        }

        InsertFont(fontName, fontSize);
    }
}

void cFontManager::Debug(void) {
    dsyslog("skindesigner: fontmanager fonts available:");
    for (map < string, map< int, cFont* > >::iterator fts = fonts.begin(); fts != fonts.end(); fts++) {
        dsyslog("skindesigner: FontName %s", fts->first.c_str());
        for (map<int, cFont*>::iterator ftSizes = (fts->second).begin(); ftSizes != (fts->second).end(); ftSizes++) {
            int confHeight = ftSizes->first;
            int realHeight = (ftSizes->second)->Height();
            dsyslog("skindesigner: fontSize %d, fontHeight %d, ratio %f", confHeight, realHeight, (double)confHeight / (double)realHeight);
        }
    }
}

void cFontManager::ListAvailableFonts(void) {
    cStringList availableFonts;
    cFont::GetAvailableFontNames(&availableFonts);
    int numFonts = availableFonts.Size();
    esyslog("skindesigner: %d Fonts available:", numFonts);
    for (int i=0; i<numFonts; i++) {
        esyslog("skindesigner: font %d: %s", i, availableFonts[i]);
    }
}

void cFontManager::DeleteFonts() {
    cMutexLock MutexLock(&mutex);
    for(map<string, map<int,cFont*> >::iterator it = fonts.begin(); it != fonts.end(); it++) {
        for(map<int,cFont*>::iterator it2 = (it->second).begin(); it2 != (it->second).end(); it2++) {
            delete it2->second;
        }
    }
    fonts.clear();
}

int cFontManager::Width(string fontName, int fontSize, const char *text) {
    cMutexLock MutexLock(&mutex);
    if (!text)
        return 0;
    cFont *font = GetFont(fontName, fontSize);
    //if not already cached, load it new
    if (!font)
        InsertFont(fontName, fontSize);
    font = GetFont(fontName, fontSize);
    if (!font)
        return 0;
    int width = font->Width(text);
    return width;
}

int cFontManager::Height(string fontName, int fontSize) {
    cMutexLock MutexLock(&mutex);
    cFont *font = GetFont(fontName, fontSize);
    //if not already cached, load it new
    if (!font)
        InsertFont(fontName, fontSize);
    font = GetFont(fontName, fontSize);
    if (!font)
        return 0;
    return font->Height();
}

cFont *cFontManager::Font(string fontName, int fontSize) {
    cMutexLock MutexLock(&mutex);
    cFont *font = GetFont(fontName, fontSize);
    //if not already cached, load it new
    if (!font)
        InsertFont(fontName, fontSize);
    font = GetFont(fontName, fontSize);
    return font;
}

cFont *cFontManager::FontUncached(string fontName, int fontSize) {
    cMutexLock MutexLock(&mutex);
    cFont *font = CreateFont(fontName, fontSize);
    return font;
}

/********************************************************************************
* Private Functions 
********************************************************************************/

cFont *cFontManager::CreateFont(string name, int size) {
    cMutexLock MutexLock(&mutex);
    cFont *fontTmp = cFont::CreateFont(name.c_str(), size);
    if (!fontTmp)
        fontTmp = cFont::CreateFont(Setup.FontOsd, size);
    int realHeight = fontTmp->Height();
    delete fontTmp;
    cFont *font = cFont::CreateFont(name.c_str(), (double)size / (double)realHeight * (double)size);
    if (!font)
        font = cFont::CreateFont(Setup.FontOsd, (double)size / (double)realHeight * (double)size);
    return font;
}

void cFontManager::InsertFont(string name, int size) {
    cFont *newFont = CreateFont(name, size);
    if (!newFont)
        return;
    map < string, map< int, cFont* > >::iterator hit = fonts.find(name);
    if (hit != fonts.end()) {
        (hit->second).insert(pair<int, cFont*>(size, newFont));
    } else {
        map<int, cFont*> fontsizes;
        fontsizes.insert(pair<int, cFont*>(size, newFont));
        fonts.insert(pair<string, map<int, cFont*> >(name, fontsizes));
    }
}

cFont *cFontManager::GetFont(string name, int size) {
    map< string, map<int,cFont*> >::iterator hitName = fonts.find(name);
    if (hitName == fonts.end())
        return NULL;
    map<int,cFont*>::iterator hitSize = (hitName->second).find(size);
    if (hitSize == (hitName->second).end())
        return NULL;
    return hitSize->second;
}

int cFontManager::GetFontHeight(const char *name, int height, int charWidth) {
    FT_Library library;
    FT_Face face;
    cString fontFileName = cFont::GetFontFileName(name);
    
    int descender = 0;
    int y_ppem = 0;
    int error = FT_Init_FreeType(&library);
    if (error) return 0;
    error = FT_New_Face(library, fontFileName, 0, &face);
    if (error) return 0;
    error = FT_Set_Char_Size(face, charWidth * 64, height * 64, 0, 0);
    if (error) return 0;

    descender = face->size->metrics.descender/64;
    y_ppem = face->size->metrics.y_ppem;
    int realHeight = y_ppem + descender;

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return realHeight;
}
