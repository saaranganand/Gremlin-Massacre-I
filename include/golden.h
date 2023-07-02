#ifndef GOLDEN_H_
#define GOLDEN_H_

#include <raylib.h>
#include <actor.h>
#include <coin.h>
#include <player.h>
#include <timer.h>

struct Golden : Actor {

    Timer invincibilityTimer;

    Sound hurt;
    Sound explosion;

    Golden(float x = 0.f, float y = 0.f);

    void explode(std::vector<Coin>& coins);
    void takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins);

    void update(Map map, Player& player, std::vector<Coin>& coins, float dt);
    void draw(bool debugging, float dt);

    void kill();
};

#endif