#ifndef PLAYER_H_
#define PLAYER_H_

#include <raylib.h>
#include <defs.h>
#include <actor.h>
#include <collider.h>
#include <map.h>
#include <ui.h>
#include <string>
#include <animation.h>
#include <timer.h>
#include <attack.h>

#define EARLY_JUMP_TIME .1f
#define LATE_JUMP_TIME .1f

struct levelData {
    float bonfireX;
    float bonfireY;
    int width;
    int height;
};

struct Player: Actor {
    Timer earlyJumpTimer;
    Timer lateJumpTimer;

    float swingKB;
    float spikeKB;

    float pogoKB;
    float pogoDeAccel;
    
    Timer invincibilityTimer;

    int coins;
    int estus;
    int maxEstus;

    Sound hurt;
    Sound attack;
    Sound hitWall;
    Sound coin;

    std::map<std::string, levelData> bonfires;
    std::string currentBonfire;

    void resetDeath();

    void loadAnimationsAtacks();

    void takeDamage(int damage, float knockback, Vector2 KB_dir);

    Player(float x = 0.f, float y = 0.f);

    void handleJump(KeyboardKey jumpKey, Map map, float dt);

    bool handleMapDamage(Map map);

    bool checkBonfire(Map map);

    bool checkLadder(Map map);

    void update(Map map, UI ui, float dt);
    void draw(bool debugging, float dt);
    void drawHP();

    void kill();
};

#endif