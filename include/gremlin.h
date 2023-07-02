#ifndef GREMLIN_H_
#define GREMLIN_H_

#include <raylib.h>
#include <defs.h>
#include <actor.h>
#include <player.h>
#include <animation.h>
#include <attack.h>
#include <coin.h>

enum GremlinState { IDLE = 0, RUNLEFT, RUNRIGHT, FIGHT, SCARED };

struct Gremlin : Actor {
    GremlinState state;

    float aggroRange;
    float attackRange;

    Timer scaredTimer;
    Timer scaredRunTimer;

    Timer invincibilityTimer;

    Sound action;
    Sound hurt;
    Sound explosion;
    Sound walk;

    Gremlin(float x = 0.f, float y = 0.f);

    void gremlinJump();

    void explode(std::vector<Coin>& coins);
    void takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins);

    void update(Map map, Player& player, std::vector<Coin>& coins, float dt);
    void draw(bool debugging, float dt);

    void kill();
};

#endif