#include <../include/raylib.h>
#include <../include/game.h>

#include <stdio.h>

Game::Game() {
    close = false;

    state = MENU;
    
    ui = UI();

    map = Map();
    load_tutorial(7, 6);

    player = Player(map.startTile.x * map.tileSize, map.startTile.y * map.tileSize);
    player.bonfires["tutorial"] = {14, 6, 98, 23};
    player.currentBonfire = "tutorial";

    camera.target = player.position;
    camera.offset = { SCREEN_W /2.f, SCREEN_H / 2.f };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;

    dt = 0.016667f;

    debugging = true;

    coinTexture = loadTextureUnloadImage("assets/coin.png");
    estusTex = loadTextureUnloadImage("assets/estus.png");
    backTex = loadTextureUnloadImage("assets/bgimg.png");

    YOUDEAD = LoadSound("assets/youdied.mp3");

    youdiedFrames = 0;
}

void Game::update() {
    player.update(map, ui, dt);
    if (player.health <= 0) {
        state = YOUDIED;
        return;
    }
    for (Gremlin& a : gremlins) {
        a.update(map, player, coins, dt);
    }
    updateCameraToMap(camera, player, map);
    int i = 0;
    for (Coin& c : coins) {
        if (c.update(player, map, dt)) {
            coins.erase(coins.begin() + i);
            i--;
        }
        i++;
    }
    Game::transitionStage();

    if (IsKeyPressed(ui.pause)) state = MENU;
}

void Game::draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    DrawTexturePro(backTex, {0, 0, 1920, 1080}, {0, -100, 1920, 1080}, ZERO, 0.f, WHITE);

    BeginMode2D(camera);

    map.draw(camera);
    //if (debugging) drawGrid();

    player.draw(debugging, dt);
    for (Gremlin a : gremlins) {
        a.draw(debugging, dt);
    }

    for (Coin c : coins) {
        c.draw(coinTexture);
    }

    EndMode2D();

    player.drawHP();
    DrawTexturePro(coinTexture, {0, 0, 46, 52}, { 20, 65, 50, 60}, ZERO, 0.f, WHITE);
    DrawTexturePro(estusTex, {0, 0, 34, 34}, { 5, 120, 80, 80}, ZERO, 0.f, WHITE);

    //if (debugging) drawCameraCrosshair();

    EndDrawing();
}

void Game::youDiedUpdate() {
    if (IsKeyPressed(ui.accept)) {
        std::string path = "levels/" + player.currentBonfire + ".out";
        printf("%s\n", path.c_str());
        map.reset();

        levelData bonfireData = player.bonfires[player.currentBonfire];
        
        float x = bonfireData.bonfireX;
        float y = bonfireData.bonfireY;

        map.reset();

        if (player.currentBonfire == "tutorial") {
            load_tutorial(x, y);
        } else if (player.currentBonfire == "room1") {
            load_1(x, y);
        } else if (player.currentBonfire == "room2") {
            load_2(x, y);
        } else if (player.currentBonfire == "room3") {
            load_3(x, y);
        } else if (player.currentBonfire == "room4") {
            load_4(x, y);
        } else if (player.currentBonfire == "room5") {
            load_5(x, y);
        } else if (player.currentBonfire == "room6") {
            load_6(x, y);
        }
        player.position = {bonfireData.bonfireX * map.tileSize, bonfireData.bonfireY * map.tileSize};
        state = PLAY;
        player.resetDeath();
    }

}

void Game::youDiedDraw() {
    BeginDrawing();

    ClearBackground(BLACK);

    DrawTexturePro(backTex, {0, 0, 1920, 1080}, {0, -100, 1920, 1080}, ZERO, 0.f, WHITE);

    BeginMode2D(camera);

    map.draw(camera);

    //if (debugging) drawGrid();

    player.draw(debugging, dt);
    
    for (Gremlin a : gremlins) {
        a.draw(debugging, dt);
    }

    for (Coin c : coins) {
        c.draw(coinTexture);
    }

    EndMode2D();

    player.drawHP();
    DrawTexturePro(coinTexture, {0, 0, 46, 52}, { 20, 65, 50, 60}, ZERO, 0.f, WHITE);
    DrawTexturePro(estusTex, {0, 0, 34, 34}, { 5, 120, 80, 80}, ZERO, 0.f, WHITE);

    
    if (youdiedFrames < 255) youdiedFrames++;

    Color col = BLACK;
    col.a = youdiedFrames;

    Color col2 = RED;
    col2.a = youdiedFrames;

    if (col2.a > 255) col2.a = 255;
    if (col.a > 255) col.a = 255;

    float hei = 400;

    DrawRectangle(0, (SCREEN_H - hei) / 2.f, SCREEN_W, hei, col);
    DrawText("YOU DIED", (SCREEN_W - MeasureText("YOU DIED", 200)) / 2.f, (SCREEN_H - hei) / 2.f + 100, 200, col2);
    DrawText("Press z", (SCREEN_W - MeasureText("Press z", 100)) / 2.f, (SCREEN_H - hei) / 2.f + 100 + 200, 100, col2);

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

void Game::transitionStage() {
    // Loop through all cells that player currently lies in.
    int leftColumn = player.left() / map.tileSize;
    int rightCloumn = player.right() / map.tileSize;

    int topRow = player.top() / map.tileSize;
    int bottomRow = player.bottom() / map.tileSize;

    clamp(leftColumn, 0, map.width - 1);
    clamp(rightCloumn, 0, map.width - 1);
    clamp(topRow, 0, map.height - 1);
    clamp(bottomRow, 0, map.height - 1);

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == TRANSITION) {
                if (map.mapName == "tutorial") {
                    map.reset();
                    load_1(1, 25);
                    player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    
                }
                else if (map.mapName == "room1") {
                    if ((player.position.x / map.tileSize) < 3) {
                        map.reset();
                        load_tutorial(96, 8);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    } else {
                        map.reset();
                        load_2(2, 27);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    }
                }
                else if (map.mapName == "room2") {
                    if ((player.position.x / map.tileSize) < 3) {
                        map.reset();
                        load_1(142, 25);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    }
                    else if ((player.position.y / map.tileSize) < 3) {
                        map.reset();
                        load_3(179, 152);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                        player.CH_velocity.y = -2000.f;
                    }
                    else if ((player.position.y / map.tileSize) > 60) {
                        map.reset();
                        load_5(1, 52);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    } else {
                        map.reset();
                        load_5(1, 13);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    }
                }
                else if (map.mapName == "room3") {
                    if ((player.position.y / map.tileSize) < 78) { // top
                        map.reset();
                        load_4(0, 0);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    } else {
                        map.reset();
                        load_2(27, 2);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    }
                }
                else if (map.mapName == "room5") {
                    if ((player.position.x / map.tileSize) > 30) {
                        map.reset();
                        load_6(0, 0);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    } else if ((player.position.y / map.tileSize) > 36) {
                        map.reset();
                        load_2(89, 75);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    } else {
                        map.reset();
                        load_2(89, 52);
                        player.position = {map.startTile.x * map.tileSize, map.startTile.y * map.tileSize};
                    }
                }

                //load entities
                return;
            }
        }
    }
}

void Game::emptyEntites() {
    for (Gremlin a : gremlins) {
        a.kill();
    }
    gremlins.clear();
}

void Game::kill() {
    UnloadTexture(coinTexture);
    UnloadTexture(backTex);
    UnloadTexture(estusTex);
    UnloadSound(YOUDEAD);
    emptyEntites();
}

void Game::load_tutorial(float x, float y) {
    map.assignMap("levels/tutorial.out", 98, 23, {x, y}, {14, 6});
    map.mapName = "tutorial";

    gremlins.push_back(Gremlin(69 * map.tileSize, 8 * map.tileSize));
}

void Game::load_1(float x, float y) {
    map.assignMap("levels/room1.out", 144, 48, {x, y});
    map.mapName = "room1";

    gremlins.push_back(Gremlin(11 * map.tileSize, 25 * map.tileSize));
    gremlins.push_back(Gremlin(27 * map.tileSize, 25 * map.tileSize));
    gremlins.push_back(Gremlin(119 * map.tileSize, 25 * map.tileSize));

    gremlins.push_back(Gremlin(80 * map.tileSize, 31 * map.tileSize));
    gremlins.push_back(Gremlin(89 * map.tileSize, 30 * map.tileSize));

    gremlins.push_back(Gremlin(75 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(76 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(77 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(78 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(79 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(80 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(81 * map.tileSize, 22 * map.tileSize));
    gremlins.push_back(Gremlin(82 * map.tileSize, 22 * map.tileSize));
}

void Game::load_2(float x, float y) {
    map.assignMap("levels/room2.out", 91, 90, {x, y}, {16, 25});
    map.mapName = "room2";

}

void Game::load_3(float x, float y) {
    map.assignMap("levels/room3.out", 200, 155, {x, y}, {36, 124});
    map.mapName = "room3";

}

void Game::load_4(float x, float y) {
    
}

void Game::load_5(float x, float y) {
    map.assignMap("levels/room5.out", 68, 80, {x, y}, {34, 68});
    map.mapName = "room5";

}

void Game::load_6(float x, float y) {

}