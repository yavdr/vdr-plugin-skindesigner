#ifndef __NOPACITY_IMAGECACHE_H
#define __NOPACITY_IMAGECACHE_H

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <Magick++.h>
#include <vector>
#include "imagemagickwrapper.h"
#include "../libtemplate/templatefunction.h"

using namespace Magick;

class cImageCache : public cImageMagickWrapper {
public:
    cImageCache();
    ~cImageCache();
    void Lock(void) { mutex.Lock(); }
    void Unlock(void) { mutex.Unlock(); }
    //channel logos
    void CacheLogo(int width, int height);
    cImage *GetLogo(string channelID, int width, int height);
    bool LogoExists(string channelID);
    cImage *GetSeparatorLogo(string name, int width, int height);
    bool SeparatorLogoExists(string name);
    //icons
    void CacheIcon(eImageType type, string path, int width, int height);
    cImage *GetIcon(eImageType type, string name, int width, int height);
    string GetIconName(string label);
    //skinparts
    void CacheSkinpart(string path, int width, int height);
    cImage *GetSkinpart(string name, int width, int height);
    //helpers
    void Clear(void);
    void Debug(bool full);
    void GetIconCacheSize(int &num, int &size);
    void GetLogoCacheSize(int &num, int &size);
    void GetSkinpartsCacheSize(int &num, int &size);
private:
    static cMutex mutex;
    static string items[16];
    cImage *tempStaticLogo;
    map<string, cImage*> iconCache;
    map<string, cImage*> channelLogoCache;
    map<string, cImage*> skinPartsCache;
    bool LoadIcon(eImageType type, string name);
    bool LoadLogo(const cChannel *channel);
    bool LoadSeparatorLogo(string name);
    bool LoadSkinpart(string name);
};

#endif //__NOPACITY_IMAGECACHE_H
