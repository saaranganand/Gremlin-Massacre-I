#ifndef ANIMATION_H_
#define ANIMTION_H_

#include <raylib.h>
#include <map>
#include <vector>
#include <string>

struct Animation {
    Texture2D sheet;
    Rectangle source;

    float destWidth;
    float destHeight;

    float sourceWidth;
    float sourceHeight;

    bool loop;
    int fps;
    int frames;

    float xOffset;
    float yOffset;
    float flipXOffset;

    Animation(int fms = 1, float sW = 0.f, float sH = 0.f, float dW = 0.f, float dH = 0.f, bool lop = false, float fXO = 0.f, float xO = 0.f, float yO = 0.f, int f = 10);

    Rectangle getDestFrame(float x, float y);

    void kill();
};

struct AnimationHandler {
    Animation* current;
    std::map<std::string, Animation> animations;

    bool flipX;
    bool done;

    int frame;
    float time;

    AnimationHandler();

    void setAnim(const char* name);

    void update(float dt);
    void draw(float x, float y, Color tint);
    void kill();
};

#endif