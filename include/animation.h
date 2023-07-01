#ifndef ANIMATION_H_
#define ANIMTION_H_

#include <raylib.h>
#include <map>

struct Animation {
    Texture2D sheet;

    int frames;
    int currentFrame;
    float timeCounter;
    float frameTime;

    float width;
    float height;

    bool loop;

    Rectangle source;
    float origWidth;

    Animation(Texture2D tex = {}, int fms = 0, float frameT = 0.f, bool loop = false, float w = 0.f, float h = 0.f);

    void tick();
    void update(float dt);
    void draw(Rectangle dest, Color tint);
    void kill();
};

#endif