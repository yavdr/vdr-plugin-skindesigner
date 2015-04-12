#include "animation.h"

using namespace std;

cAnimation::cAnimation(eAnimType animType, int animFreq, cRect &pos, int layer) : cPixmapContainer(1) {
    this->animType = animType;
    this->animFreq = animFreq;
    this->pos = pos;
    this->layer = layer;
    blinkOn = true;
}

cAnimation::~cAnimation() {
}

void cAnimation::Action(void) {
    CreatePixmap(0, layer+1, pos);
    bool init = true;
    while (Running()) {
        if (animType == atBlink) {
            if (!blinkOn) {
                Fill(0, clrTransparent);
                blinkOn = true;
            } else {
                DrawBlink();
            }
        } else if (animType == atAnimated) {
            esyslog("skindesigner: animationType atAnimated not implemented");
        }
        if (init)
            FadeIn();
        init = false;
        DoFlush();
        DoSleep(animFreq);
    }
}

void cAnimation::Stop(void) {
    CancelSave();
}

/********************************************************************************************
* cAnimatedImage
********************************************************************************************/
cAnimatedImage::cAnimatedImage(eAnimType animType, int animFreq, cRect &pos, int layer) : cAnimation(animType, animFreq, pos, layer) {
    image = NULL;
}

cAnimatedImage::~cAnimatedImage() {
    esyslog("skindesigner: killing animation");
}

void cAnimatedImage::DrawBlink(void) {
    blinkOn = false;
    if (!image)
        return;

    cPoint posImage(0,0);
    if (Running())
        DrawImage(0, posImage, *image);
}

/********************************************************************************************
* cAnimatedText
********************************************************************************************/
cAnimatedText::cAnimatedText(eAnimType animType, int animFreq, cRect &pos, int layer) : cAnimation(animType, animFreq, pos, layer) {
    text = "";
    fontName = "";
    fontSize = 1;
    fontColor = clrTransparent;
}

cAnimatedText::~cAnimatedText() {
}

void cAnimatedText::DrawBlink(void) {
    blinkOn = false;
    if (text.size() == 0)
        return;

    cPoint posText(0,0);
    if (Running())
        DrawText(0, posText, text.c_str(), fontColor, clrTransparent, fontName, fontSize);
}

/********************************************************************************************
* cAnimatedOsdObject
********************************************************************************************/
cAnimatedOsdObject::cAnimatedOsdObject(eFuncType type, eAnimType animType, int animFreq, cRect &pos, int layer) : cAnimation(animType, animFreq, pos, layer) {
    this->type = type;
    color = 0x00000000;
    quadrant = 0;
}

cAnimatedOsdObject::~cAnimatedOsdObject() {
}

void cAnimatedOsdObject::DrawBlink(void) {
    blinkOn = false;
    cRect posObject(0, 0, pos.Width(), pos.Height());
    if (Running()) {
        if (type == ftDrawRectangle) {
            DrawRectangle(0, posObject, color);
        } else if (type == ftDrawEllipse) {
            DrawEllipse(0, posObject, color, quadrant);
        } else if (type == ftDrawSlope) {
            DrawSlope(0, posObject, color, quadrant);
        }
    }
}
