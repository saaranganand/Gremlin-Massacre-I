#ifndef FROG_H_
#define FROG_H_

#include <raylib.h>
#include <actor.h>
#include <player.h>
#include <animation.h>
#include <coin.h>

struct Frog : Actor {
    float AggroRange;
    
    Sound action;
    Sound hurt;
    Sound explosion;

    Frog(float x = 0.f, float y = 0.f);

    void gremlinJump();

    void explode(std::vector<Coin>& coins);
    void takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins);

    void update(Map map, Player& player, std::vector<Coin>& coins, float dt);
    void draw(bool debugging, float dt);

    void kill();
};

#endif