#define __STL_CONFIG_H
#include "pixmapcontainer.h"
#include "../config.h"

cMutex cPixmapContainer::mutex;
cOsd *cPixmapContainer::osd = NULL;
eFlushState cPixmapContainer::flushState = fsOpen;

cPixmapContainer::cPixmapContainer(int numPixmaps) {
    this->numPixmaps = numPixmaps;
    pixContainerInit = true;
    mutex.Lock();
    pixmaps = new cPixmap*[numPixmaps];
    pixmapsTransparency = new int[numPixmaps];
    for(int i=0; i < numPixmaps; i++) {
        pixmaps[i] = NULL;
        pixmapsTransparency[i] = 0;
    }
    pixmapsLayer = NULL;
    mutex.Unlock();
    checkRunning = false;
    fadeTime = 0;
    deleteOsdOnExit = false;
}

cPixmapContainer::~cPixmapContainer(void) {
    for (int i=0; i < numPixmaps; i++) {
        mutex.Lock();
        if (pixmaps[i] && osd) {
            osd->DestroyPixmap(pixmaps[i]);
            pixmaps[i] = NULL;
        }
        mutex.Unlock();
    }
    delete[] pixmaps;
    delete[] pixmapsTransparency;
    if (pixmapsLayer)
        delete[] pixmapsLayer;

    if (deleteOsdOnExit && osd) {
        mutex.Lock();
        delete osd;
        osd = NULL;
        mutex.Unlock();
    }
    flushState = fsOpen;
}

bool cPixmapContainer::CreateOsd(int Left, int Top, int Width, int Height) {
    if (osd) {
        return false;
    }
    cOsd *newOsd = cOsdProvider::NewOsd(Left, Top);
    if (newOsd) {
        tArea Area = { 0, 0, Width - 1, Height - 1,  32 };
        if (newOsd->SetAreas(&Area, 1) == oeOk) {
            osd = newOsd;
            return true;
        }
    }
    return false;
}

void cPixmapContainer::LockFlush(void) {
    flushState = fsLock; 
}

void cPixmapContainer::OpenFlush(void) { 
    flushState = fsOpen; 
}


void cPixmapContainer::DoFlush(void) {
    cMutexLock MutexLock(&mutex);
    if (!osd || (checkRunning && !Running()))
        return;
    if (flushState == fsOpen) {
        osd->Flush();
    }
}

void cPixmapContainer::HidePixmaps(void) {
    cMutexLock MutexLock(&mutex);
    pixmapsLayer = new int[numPixmaps];
    for(int i=0; i < numPixmaps; i++) {
        if (!pixmaps[i]) {
            pixmapsLayer[i] = 0;
            continue;
        }
        pixmapsLayer[i] = pixmaps[i]->Layer();
        pixmaps[i]->SetLayer(-1);
    }
}

void cPixmapContainer::ShowPixmaps(void) {
    cMutexLock MutexLock(&mutex);
    if (!pixmapsLayer)
        return;
    for(int i=0; i < numPixmaps; i++) {
        if (!pixmaps[i])
            continue;
        pixmaps[i]->SetLayer(pixmapsLayer[i]);
    }
}

/******************************************************************************************************
* Protected Functions
******************************************************************************************************/

bool cPixmapContainer::PixmapExists(int num) {
    cMutexLock MutexLock(&mutex);
    if (pixmaps[num])
        return true;
    return false;
}

void cPixmapContainer::CreatePixmap(int num, int Layer, const cRect &ViewPort, const cRect &DrawPort) {
    cMutexLock MutexLock(&mutex);
    if (!osd || (checkRunning && !Running()))
        return;
    pixmaps[num] = osd->CreatePixmap(Layer, ViewPort, DrawPort);
    if (!pixmaps[num])
        return;
    pixmaps[num]->Fill(clrTransparent);
    if (pixContainerInit && fadeTime) {
        pixmaps[num]->SetAlpha(0);
    } else if (pixmapsTransparency[num] > 0) {
        int alpha = (100 - pixmapsTransparency[num])*255/100;
        pixmaps[num]->SetAlpha(alpha);
    }
}

bool cPixmapContainer::DestroyPixmap(int num) {
    cMutexLock MutexLock(&mutex);
    if (pixmaps[num] && osd) {
        osd->DestroyPixmap(pixmaps[num]);
        pixmaps[num] = NULL;
        return true;
    }
    return false;
}

void cPixmapContainer::DrawText(int num, const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, std::string fontName, int fontSize) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    fontManager->Lock();
    cFont *font = fontManager->Font(fontName, fontSize);
    if (font)
        pixmaps[num]->DrawText(Point, s, ColorFg, ColorBg, font);
    fontManager->Unlock();    
}


void cPixmapContainer::DrawRectangle(int num, const cRect &Rect, tColor Color) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->DrawRectangle(Rect, Color);
}

void cPixmapContainer::DrawEllipse(int num, const cRect &Rect, tColor Color, int Quadrants) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->DrawEllipse(Rect, Color, Quadrants);
}

void cPixmapContainer::DrawSlope(int num, const cRect &Rect, tColor Color, int Type) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->DrawSlope(Rect, Color, Type);
}

void cPixmapContainer::DrawImage(int num, const cPoint &Point, const cImage &Image) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->DrawImage(Point, Image);
}

void cPixmapContainer::DrawBitmap(int num, const cPoint &Point, const cBitmap &Bitmap, tColor ColorFg, tColor ColorBg, bool Overlay) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->DrawBitmap(Point, Bitmap, ColorFg, ColorBg, Overlay);    
}

void cPixmapContainer::Fill(int num, tColor Color) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->Fill(Color);
}

void cPixmapContainer::SetAlpha(int num, int Alpha) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->SetAlpha(Alpha);
}

void cPixmapContainer::SetTransparency(int num, int Transparency) {
    if (Transparency < 0 && Transparency > 100)
        return;
    pixmapsTransparency[num] = Transparency;
}

void cPixmapContainer::SetLayer(int num, int Layer) {
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->SetLayer(Layer);
}

void cPixmapContainer::SetViewPort(int num, const cRect &rect) {
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->SetViewPort(rect);    
}

int cPixmapContainer::Layer(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    return pixmaps[num]->Layer();
}

void cPixmapContainer::Pos(int num, cPoint &pos) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pos.SetX(pixmaps[num]->ViewPort().X());
    pos.SetY(pixmaps[num]->ViewPort().Y());
}

int cPixmapContainer::Width(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int width = pixmaps[num]->ViewPort().Width();
    return width;
}

int cPixmapContainer::Height(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int height = pixmaps[num]->ViewPort().Height();
    return height;
}

int cPixmapContainer::DrawportWidth(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int width = pixmaps[num]->DrawPort().Width();
    return width;
}

int cPixmapContainer::DrawportHeight(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int height = pixmaps[num]->DrawPort().Height();
    return height;
}

int cPixmapContainer::DrawportX(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int x = pixmaps[num]->DrawPort().X();
    return x;
}

int cPixmapContainer::DrawportY(int num) {
    if (checkRunning && !Running())
        return 0;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return 0;
    int y = pixmaps[num]->DrawPort().Y();
    return y;
}

void cPixmapContainer::SetDrawPortPoint(int num, const cPoint &Point) {
    if (checkRunning && !Running())
        return;
    cMutexLock MutexLock(&mutex);
    if (!pixmaps[num])
        return;
    pixmaps[num]->SetDrawPortPoint(Point);
}

/***************************************************************************
* HELPERS -- do not access the pixmaps array directly, use wrapper functions
*            to ensure that a proper lock is set before accessing pixmaps
****************************************************************************/

void cPixmapContainer::FadeIn(void) {
    if (!fadeTime)
        return;
    uint64_t Start = cTimeMs::Now();
    int FadeFrameTime = fadeTime / 10;
    while (Running()) {
        uint64_t Now = cTimeMs::Now();
        double t = min(double(Now - Start) / fadeTime, 1.0);
        int Alpha = t * ALPHA_OPAQUE;
        for (int i = 0; i < numPixmaps; i++) {
            if (!PixmapExists(i))
                continue;
            if (pixmapsTransparency[i] > 0) {
                int alpha = (100 - pixmapsTransparency[i])*Alpha/100;
                SetAlpha(i, alpha);
            } else {
                SetAlpha(i, Alpha);
            }
        }
        DoFlush();
        int Delta = cTimeMs::Now() - Now;
        if (Running() && (Delta < FadeFrameTime))
            cCondWait::SleepMs(FadeFrameTime - Delta);
        if ((int)(Now - Start) > fadeTime)
            break;
    }
}

void cPixmapContainer::FadeOut(void) {
    if (!fadeTime)
        return;
    uint64_t Start = cTimeMs::Now();
    int FadeFrameTime = fadeTime / 10;
    while (true) {
        uint64_t Now = cTimeMs::Now();
        double t = min(double(Now - Start) / fadeTime, 1.0);
        int Alpha = (1 - t) * ALPHA_OPAQUE;
        for (int i = 0; i < numPixmaps; i++) {
            if (!PixmapExists(i))
                continue;
            if (pixmapsTransparency[i] > 0) {
                int alpha = (100 - pixmapsTransparency[i])*Alpha/100;
                SetAlpha(i, alpha);
            } else {
                SetAlpha(i, Alpha);
            }
        }
        DoFlush();
        int Delta = cTimeMs::Now() - Now;
        if (Delta < FadeFrameTime)
            cCondWait::SleepMs(FadeFrameTime - Delta);
        if ((int)(Now - Start) > fadeTime)
            break;
    }
}

/*****************************************
* scrollSpeed: 1 slow
*              2 medium
*              3 fast
******************************************/
void cPixmapContainer::ScrollHorizontal(int num, int scrollDelay, int scrollSpeed, int scrollMode) {
    bool carriageReturn = (scrollMode == 1) ? true : false;
    
    int scrollDelta = 1;
    int drawPortX;

    int FrameTime = 0;
    if (scrollSpeed == 1)
        FrameTime = 50;
    else if (scrollSpeed == 2)
        FrameTime = 30;
    else
        FrameTime = 15;
    if (!Running())
        return;
    int maxX = DrawportWidth(num) - Width(num);
    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            DoSleep(scrollDelay);
            doSleep = false;
        }
        if (!Running())
            return;
        uint64_t Now = cTimeMs::Now();
        drawPortX = DrawportX(num);
        drawPortX -= scrollDelta;

        if (abs(drawPortX) > maxX) {
            DoSleep(scrollDelay);
            if (carriageReturn)
                drawPortX = 0;
            else {
                scrollDelta *= -1;
                drawPortX -= scrollDelta;
            }
            doSleep = true;
        }
        if (!carriageReturn && (drawPortX == 0)) {
            scrollDelta *= -1;
            doSleep = true;
        }
        SetDrawPortPoint(num, cPoint(drawPortX, 0));
        int Delta = cTimeMs::Now() - Now;
        DoFlush();
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
    }
}

/*****************************************
* scrollSpeed: 1 slow
*              2 medium
*              3 fast
******************************************/
void cPixmapContainer::ScrollVertical(int num, int scrollDelay, int scrollSpeed) {
    if (!scrollSpeed)
        return;
    DoSleep(scrollDelay);
    int drawPortY;
    int FrameTime = 0;
    if (scrollSpeed == 1)
        FrameTime = 50;
    else if (scrollSpeed == 2)
        FrameTime = 30;
    else 
        FrameTime = 15;
    int maxY = DrawportHeight(num) - Height(num);
    bool doSleep = false;
    while (Running()) {
        if (doSleep) {
            doSleep = false;
            DoSleep(scrollDelay);
        }
        uint64_t Now = cTimeMs::Now();
        drawPortY = DrawportY(num);
        drawPortY -= 1;
        if (abs(drawPortY) > maxY) {
            doSleep = true;
            DoSleep(scrollDelay);
            drawPortY = 0;
        }
        SetDrawPortPoint(num, cPoint(0, drawPortY));
        if (doSleep) {
            DoSleep(scrollDelay);
        }
        int Delta = cTimeMs::Now() - Now;
        DoFlush();
        if (Running() && (Delta < FrameTime))
            cCondWait::SleepMs(FrameTime - Delta);
    }
}

void cPixmapContainer::CancelSave(void) {
    Cancel(-1);
    while (Active())
        cCondWait::SleepMs(10);    
}

void cPixmapContainer::DoSleep(int duration) {
    int sleepSlice = 10;
    for (int i = 0; Running() && (i*sleepSlice < duration); i++)
        cCondWait::SleepMs(sleepSlice);
}

void cPixmapContainer::DrawBlendedBackground(int num, int xStart, int width, tColor color, tColor colorBlending, bool fromTop) {
    int height = Height(num);
    int numSteps = 16;
    int alphaStep = 0x0F;
    int alpha = 0x00;
    int step, begin, end;
    if (fromTop) {
        step = 1;
        begin = 0;
        end = numSteps;
    } else {
        step = -1;
        begin = height;
        end = height - numSteps;
    }
    tColor clr;
    bool cont = true;
    for (int i = begin; cont; i = i + step) {
        clr = AlphaBlend(color, colorBlending, alpha);
        DrawRectangle(num, cRect(xStart,i,width,1), clr);
        alpha += alphaStep;
        if (i == end)
            cont = false;
    }
}

void cPixmapContainer::DrawRoundedCorners(int num, int radius, int x, int y, int width, int height) {
    if (radius > 2) {
        DrawEllipse(num, cRect(x, y, radius, radius), clrTransparent, -2);
        DrawEllipse(num, cRect(x + width - radius, y , radius, radius), clrTransparent, -1);
        DrawEllipse(num, cRect(x, y + height - radius, radius, radius), clrTransparent, -3);
        DrawEllipse(num, cRect(x + width - radius, y + height - radius, radius, radius), clrTransparent, -4);
    }
}

void cPixmapContainer::DrawRoundedCornersWithBorder(int num, tColor borderColor, int radius, int width, int height) {
    if (radius < 3)
        return;
    DrawEllipse(num, cRect(0,0,radius,radius), borderColor, -2);
    DrawEllipse(num, cRect(-1,-1,radius,radius), clrTransparent, -2);

    DrawEllipse(num, cRect(width-radius,0,radius,radius), borderColor, -1);
    DrawEllipse(num, cRect(width-radius+1,-1,radius,radius), clrTransparent, -1);

    DrawEllipse(num, cRect(0,height-radius,radius,radius), borderColor, -3);
    DrawEllipse(num, cRect(-1,height-radius+1,radius,radius), clrTransparent, -3);

    DrawEllipse(num, cRect(width-radius,height-radius,radius,radius), borderColor, -4);
    DrawEllipse(num, cRect(width-radius+1,height-radius+1,radius,radius), clrTransparent, -4);
}
