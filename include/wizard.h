#ifndef WIZARD_H_
#define WIZARD_H_

#include <raylib.h>
#include <actor.h>
#include <player.h>
#include <map.h>
#include <coin.h>

enum WizardState { IDLE, CASTING, FOLLOW };

struct Orb {
    Vector2 position;
    Vector2 velocity;
};

struct Beam {
    Vector2 position;
    Vector2 velocity;
};

struct Wizard : Actor {
    WizardState state;

    float followRange;

    Orb orbattack;
    Beam beamAttack;

    Sound action;
    Sound hurt;
    Sound explosion;
    Sound cast;

    Texture2D orb;
    Texture2D beam;

    Timer invincibilityTimer;
    Timer attackTimer;

    void castOrb(Vector2 dir);
    void castRay(Vector2 dir);

    void reset(float x, float y);

    Wizard(float x = 0.f, float y = 0.f);

    void explode(std::vector<Coin>& coins);
    void takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins);

    void update(Map map, Player& player, std::vector<Coin>& coins, float dt);
    void draw(bool debugging, float dt);

    void kill();
};

#endif