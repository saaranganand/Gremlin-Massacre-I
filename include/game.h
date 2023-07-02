#ifndef GAME_H_
#define GAME_H_

#include <raylib.h>
#include <defs.h>
#include <actor.h>
#include <player.h>
#include <gremlin.h>
#include <ui.h>
#include <map.h>
#include <camera.h>
#include <coin.h>
#include <frog.h>
#include <vector>
#include <utility>
#include <golden.h>

struct Game {
    bool close;

    State state;
    UI ui;
    Player player;
    Map map;
    Camera2D camera;

    Texture2D coinTexture;
    std::vector<Coin> coins;

    Texture2D backTex;
    Texture2D estusTex;

    std::vector<Gremlin> gremlins;
    std::vector<Frog> frogs;
    std::vector<Golden> goldens;

    Sound YOUDEAD;

    int youdiedFrames;

    float dt;
    int estusCost;

    bool debugging;

    Game();

    void update();
    void draw();

    void youDiedUpdate();
    void youDiedDraw();

    void shoppingUpdate();
    void shoppingDraw();

    void drawGrid();
    void drawCameraCrosshair();
    void transitionStage();

    void emptyEntites();

    void kill();
    
    void load_tutorial(float x, float y);
    void load_1(float x, float y);

    void load_2(float x, float y);

    void load_3(float x, float y);

    void load_4(float x, float y);

    void load_5(float x, float y);

    void load_6(float x, float y);
};

#endif