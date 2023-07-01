#ifndef ATTACK_H_
#define ATTACK_H_

#include <raylib.h>
#include <collider.h>
#include <defs.h>

#include <vector>

struct Attack {
    Vector2 parentPosition;
    
    Vector2 offset;
    Collider hitbox;

    bool flipOffsetX;
    bool flipOffsetY;

    bool queued;

    float time;
    int timeFramecount;
    
    float timeout;
    int timeoutFramecount;

    bool applyAffect;
    bool pogo;

    Attack(Vector2 parentPos = ZERO, float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f, float t = 0.f, float to = 0.f);
    void attempt();
    void update(Vector2 parentPos, float dt);
    void trigger(bool on);
    void draw(bool debugging);
};

#endif