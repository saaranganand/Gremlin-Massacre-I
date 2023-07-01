#ifndef GREMLIN_H_
#define GREMLIN_H_

#include <raylib.h>
#include <defs.h>
#include <actor.h>
#include <player.h>
#include <animation.h>
#include <attack.h>

struct Gremlin : Actor {
    AttackHandler atks;

    AnimationHandler anims;

    float aggroRange;
    bool aggroed;
    
    float attackRange;
    bool attackMode;

    Gremlin(float x = 0.f, float y = 0.f);

    void gremlinJump();

    void update(Map map, Player player, float dt);
    void draw(bool debugging, float dt);

    void kill();
};

#endif