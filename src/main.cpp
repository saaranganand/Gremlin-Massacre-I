#include <../include/raylib.h>
#include <../include/game.h>
#include <../include/defs.h>
#include <stdio.h>

int SCREEN_W = 900;
int SCREEN_H = 600;

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "template");
    
    Game game = Game();

    while (!WindowShouldClose() && !game.close) {
        game.dt = GetFrameTime();

        switch (game.state) {
            case MENU:
                game.state = game.ui.menuing();
                game.ui.drawMenu();
                break;

            case PLAY:
                game.update();
                game.draw();
                break;
            
            case OPTIONS:
                game.state = game.ui.optioning(game.camera.zoom, game.dt);
                game.ui.drawOptions();
                break;

            case QUIT:
                game.close = true;
                break;
        }
    }
    game.map.kill();
    game.player.kill();
    CloseWindow();
    return 0;
}