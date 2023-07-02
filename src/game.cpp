#include <../include/raylib.h>
#include <../include/game.h>

#include <stdio.h>

Game::Game() {
    close = false;

    state = MENU;
    
    ui = UI();

    map = Map();
    tutorial_assignMap(map);

    player = Player(map.startTile.x * map.tileSize, map.startTile.y * map.tileSize);
    gremlin = Gremlin(68 * map.tileSize, 6 * map.tileSize);

    camera.target = player.position;
    camera.offset = { SCREEN_W /2.f, SCREEN_H / 2.f };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;

    dt = 0.016667f;

    debugging = true;

    coinTexture = loadTextureUnloadImage("assets/coin.png");
}

void Game::update() {
    player.update(map, ui, dt);
    gremlin.update(map, player, coins, dt);

    updateCameraToMap(camera, player, map);

    int i = 0;
    for (Coin& c : coins) {
        if (c.update(player, map, dt)) {
            coins.erase(coins.begin() + i);
            i--;
        }
        i++;
    }

    if (IsKeyPressed(ui.pause)) state = MENU;
}

void Game::draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode2D(camera);

    map.draw(camera);

    //if (debugging) drawGrid();

    player.draw(debugging, dt);
    gremlin.draw(debugging, dt);

    for (Coin c : coins) {
        c.draw(coinTexture);
    }

    EndMode2D();

    player.drawHP();

    //if (debugging) drawCameraCrosshair();

    EndDrawing();
}

void Game::drawGrid() {
    for (int x = 0; x <= map.width * map.tileSize; x += map.tileSize) {
        DrawLine(x, 0.f, x, map.width * map.tileSize, BLACK);
    }

    for (int y = 0; y <= map.height * map.tileSize; y += map.tileSize) {
        DrawLine(0.f, y, map.height * map.tileSize, y, BLACK);
    }
}

void Game::drawCameraCrosshair() {
    DrawLine(0, SCREEN_H / 2.f, SCREEN_W, SCREEN_H / 2.f, GREEN);
    DrawLine(SCREEN_W / 2.f, 0, SCREEN_W / 2.f, SCREEN_H, GREEN);
}

void Game::kill() {
    UnloadTexture(coinTexture);
}