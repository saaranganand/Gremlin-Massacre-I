#ifndef COIN_H_
#define COIN_H_

#include <raylib.h>
#include <collider.h>
#include <map.h>
#include <player.h>
#include <defs.h>

struct Coin {
    Collider col;
    Vector2 position;
    Vector2 velo;

    bool grounded;
    float grav;

    Coin(Vector2 pos = {0, 0}, Vector2 vel = {0, 0});

    void collideWithVerticalStaticStage(Map map, float dt);
    void collideWithHorizontalStaticStage(Map map, float dt);

    bool update(Player& player, Map map, float dt);
    void draw(Texture2D tex);

    float left();
    float right();
    float top();
    float bottom();  
};

#endif