#ifndef PLAYER_H_
#define PLAYER_H_

#include <raylib.h>
#include <defs.h>
#include <actor.h>
#include <collider.h>
#include <map.h>
#include <ui.h>
#include <animation.h>
#include <timer.h>
#include <attack.h>

#define EARLY_JUMP_TIME .1f
#define LATE_JUMP_TIME .1f


struct Player: Actor {    
    AttackHandler atks;

    AnimationHandler anims;

    Timer earlyJumpTimer;
    Timer lateJumpTimer;

    float swingKB;
    float spikeKB;

    float pogoKB;
    float pogoDeAccel;

    
    Timer invincibilityTimer;

    Player(float x = 0.f, float y = 0.f);

    bool isSwingKnockback(Map map);
    bool isPogoKnockback(Map map);

    void handleJump(KeyboardKey jumpKey, float dt);

    void takeDamage(float KB_tile);

    bool handleMapDamage(Map map);

    void update(Map map, UI ui, float dt);
    void draw(bool debugging, float dt);
    void drawHP();

    void kill();
};

#endif