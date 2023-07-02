#ifndef ATTACK_H_
#define ATTACK_H_

#include <raylib.h>
#include <collider.h>
#include <defs.h>

#include <vector>
#include <string>
#include <map>

struct Attack {
    Collider hitbox;
    std::string anim;

    float xOffset;
    float yOffset;

    float flipXOffset;

    float duration;
    float timeout;

    Attack(std::string ani = "idle", float w = 0.f, float h = 0.f, float xO = 0.f, float yO = 0.f, float fXO = 0.f, float dur = 0.f, float timeO = 0.f);
};

struct AttackHandler {
    std::map<const char*, Attack> attacks;
    Attack* current;

    float attackTime;
    float attackLength;

    float timeoutTime;
    float timeoutLength;

    bool active;
    bool flipX;
    bool done;

    AttackHandler();

    void play(const char* name);
    void update(float x, float y, float dt);
};

#endif