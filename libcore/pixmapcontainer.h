#ifndef __PIXMAP_CONTAINER_H
#define __PIXMAP_CONTAINER_H

#include <string>
#include <vdr/plugin.h>
#include "fontmanager.h"

enum eFlushState {
    fsOpen,
    fsLock,
    fsCount,
};

class cPixmapContainer : public cThread {
private:
    static cMutex mutex;
    static cOsd *osd;
    static eFlushState flushState;
    bool pixContainerInit;
    int numPixmaps;
    cPixmap **pixmaps;
    int *pixmapsTransparency;
    int *pixmapsLayer;
    bool checkRunning;
    int fadeTime;
    int shiftTime;
    int shiftType;
    int shiftMode;
    cPoint startPos;
    bool deleteOsdOnExit;
    void ShiftInFromBorder(int frames, int frameTime);
    void ShiftOutToBorder(int frames, int frameTime);
    void ShiftInFromPoint(int frames, int frameTime);
    void ShiftOutToPoint(int frames, int frameTime);
    cRect UnionPixmaps(void);
protected:
    void SetInitFinished(void) { pixContainerInit = false; };
    bool CreateOsd(int Left, int Top, int Width, int Height);
    void DeleteOsdOnExit(bool doDelete = true) { deleteOsdOnExit = doDelete; };
    //Wrappers for access to pixmaps
    bool PixmapExists(int num);
    int NumPixmaps(void) { return numPixmaps; };
    void CreatePixmap(int num, int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null);
    bool DestroyPixmap(int num);
    void DrawText(int num, const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, std::string fontName, int fontSize);
    void DrawRectangle(int num, const cRect &Rect, tColor Color);
    void DrawEllipse(int num, const cRect &Rect, tColor Color, int Quadrants = 0);
    void DrawSlope(int num, const cRect &Rect, tColor Color, int Type);
    void DrawImage(int num, const cPoint &Point, const cImage &Image);
    void DrawBitmap(int num, const cPoint &Point, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0, bool Overlay = false);
    void Fill(int num, tColor Color);
    void SetAlpha(int num, int Alpha);
    void SetTransparency(int num, int Transparency);
    void SetLayer(int num, int Layer);
    void SetViewPort(int num, const cRect &rect);
    int Layer(int num);
    void Pos(int num, cPoint &pos);
    cRect ViewPort(int num);
    int Width(int num);
    int Height(int num);
    int DrawportWidth(int num);
    int DrawportHeight(int num);
    int DrawportX(int num);
    int DrawportY(int num);
    void SetDrawPortPoint(int num, const cPoint &Point);
    void SetCheckRunning(void) { checkRunning = true; };
    void UnsetCheckRunning(void) { checkRunning = false; };
    //HELPERS -- do not access the pixmaps array directly, use wrapper functions
    void SetFadeTime(int fade) { fadeTime = fade; };
    void SetShiftTime(int shift) { shiftTime = shift; };
    void SetShiftType(int type) { shiftType = type; };
    void SetShiftMode(int mode) { shiftMode = mode; };
    void SetStartPos(int posX, int posY) { startPos.SetX(posX); startPos.SetY(posY); };
    bool IsAnimated(void) { return (shiftTime > 0); };
    int AnimationDelay(void);
    void FadeIn(void);
    void FadeOut(void);
    void ShiftIn(void);
    void ShiftOut(void);
    void ScrollVertical(int num, int scrollDelay, int scrollSpeed);
    void ScrollHorizontal(int num, int scrollDelay, int scrollSpeed, int scrollMode);
    void CancelSave(void);
    void DoSleep(int duration);
    void DrawBlendedBackground(int num, int xStart, int width, tColor color, tColor colorBlending, bool fromTop);
    void DrawRoundedCorners(int num, int radius, int x, int y, int width, int height);
    void DrawRoundedCornersWithBorder(int num, tColor borderColor, int radius, int width, int height);
public:
    cPixmapContainer(int numPixmaps);
    virtual ~cPixmapContainer(void);
    void LockFlush(void);
    void OpenFlush(void);
    void DoFlush(void);
    void HidePixmaps(void);
    void ShowPixmaps(void);
    virtual void Action(void) {};
};

#endif //__PIXMAP_CONTAINER_H