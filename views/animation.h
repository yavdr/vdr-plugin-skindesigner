#ifndef __ANIMATION_H
#define __ANIMATION_H

#include "string"
#include "../libcore/pixmapcontainer.h"
#include "../libtemplate/template.h"

using namespace std;

class cAnimation : public cPixmapContainer {
protected:
    eAnimType animType;
    int animFreq;
    cRect pos;
    int layer;
    bool blinkOn;
    virtual void DrawBlink(void) {};
    virtual void Action(void);
public:
    cAnimation(eAnimType animType, int animFreq, cRect &pos, int layer);
    virtual ~cAnimation();
    void SetAnimationFadeTime(int fadeTime) { SetFadeTime(fadeTime); };
    virtual void Stop(void);
};

class cAnimatedImage : public cAnimation {
private:
    cImage *image;
protected:
    void DrawBlink(void);
public:
    cAnimatedImage(eAnimType animType, int animFreq, cRect &pos, int layer);
    virtual ~cAnimatedImage();
    void SetImage(cImage *i) { image = i; };
};

class cAnimatedText : public cAnimation {
private:
    string text;
    string fontName;
    int fontSize;
    tColor fontColor;
protected:
    void DrawBlink(void);
public:
    cAnimatedText(eAnimType animType, int animFreq, cRect &pos, int layer);
    virtual ~cAnimatedText();
    void SetText(string &t) { text = t; };
    void SetFont(string &font) { fontName = font; };
    void SetFontSize(int size) { fontSize = size; };
    void SetFontColor(tColor col) { fontColor = col; };
};

class cAnimatedOsdObject : public cAnimation {
private:
    eFuncType type;
    tColor color;
    int quadrant;
protected:
    void DrawBlink(void);
public:
    cAnimatedOsdObject(eFuncType type, eAnimType animType, int animFreq, cRect &pos, int layer);
    virtual ~cAnimatedOsdObject();
    void SetColor(tColor col) { color = col; };
    void SetQuadrant(int q) { quadrant = q; };
};

#endif //__ANIMATION_H