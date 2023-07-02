#ifndef GAME_H_
#define GAME_H_

#include <defs.h>
#include <player.h>
#include <gremlin.h>
#include <ui.h>
#include <map.h>
#include <camera.h>
#include <coin.h>
#include <vector>

struct Game {
    bool close;

    State state;
    UI ui;
    Player player;
    Gremlin gremlin;
    Map map;
    Camera2D camera;

    Texture2D coinTexture;
    std::vector<Coin> coins;

    Texture2D backTex;
    Texture2D estusTex;

    float dt;

    bool debugging;

    Game();

    void update();
    void draw();
    void drawGrid();
    void drawCameraCrosshair();

    void kill();
};

#endif