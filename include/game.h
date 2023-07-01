#ifndef GAME_H_
#define GAME_H_

#include <defs.h>
#include <player.h>
#include <ui.h>
#include <map.h>

struct Game {
    bool close;

    State state;
    UI ui;
    Player player;
    Map map;
    Camera2D camera;

    float dt;

    bool debugging;

    Game();

    void update();
    void draw();
    void drawGrid();
    void drawCameraCrosshair();
};

#endif