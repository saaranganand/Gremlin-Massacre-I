#include <../include/raylib.h>
#include <../include/ui.h>
#include <../include/defs.h>

#include <math.h>
#include <stdio.h>

UI::UI() {
    SetExitKey(KEY_NULL);
    
    accept = KEY_Z;
    back = KEY_X;

    pause = KEY_ESCAPE;

    up = KEY_UP;
    down = KEY_DOWN;
    left = KEY_LEFT;
    right = KEY_RIGHT;

    action = KEY_X;
    jump = KEY_Z;
    heal = KEY_C;
    dash = KEY_LEFT_SHIFT;

    menuState = 1;
    menuMax = 0;

    volume = 50;

    backG = loadTextureUnloadImage("assets/logo.png");
}

State UI::menuing() {
    menuMax = 3;
    if (IsKeyPressed(up)) menuState--;
    if (IsKeyPressed(down)) menuState++;

    // Clamp menuState to 1..max
    if (menuState < 1) menuState = menuMax;
    if (menuState > menuMax) menuState = 1;

    if (IsKeyPressed(accept)) switch (menuState) {
        case 1:
            return PLAY;
        case 2:
            return OPTIONS;
        case 3:
            return QUIT;
    }
    
    return MENU;
}

void UI::fullscreenTrigger(float& camZoom) {

    int display = GetCurrentMonitor();

    if (!IsWindowFullscreen()) {
        SCREEN_W = GetMonitorWidth(display);
        SCREEN_H = GetMonitorHeight(display);
        SetWindowSize(SCREEN_W, SCREEN_H);

        fullscreen = true;
        camZoom = 1.5f;
    } else {
        SCREEN_W = 900;
        SCREEN_H = 600;
        SetWindowSize(SCREEN_W, SCREEN_H);

        fullscreen = false;
        camZoom = 1.f;
    }

    ToggleFullscreen();
    SetWindowSize(SCREEN_W, SCREEN_H);
}

State UI::optioning(float& camZoom, float dt) {
    menuMax = 3;
    if (IsKeyPressed(up)) menuState--;
    if (IsKeyPressed(down)) menuState++;

    // Clamp menuState to 1..max
    if (menuState < 1) menuState = menuMax;
    if (menuState > menuMax) menuState = 1;

    if (menuState == 1) {
        if (IsKeyPressed(left)) volume--;
        if (IsKeyPressed(right)) volume++;
    } else if (menuState == 2) {
        if (IsKeyPressed(accept)) {
            fullscreenTrigger(camZoom);
        }
    }

    // Control volume in fifths
    if (volume < 0) volume = 0;
    if (volume > 5) volume = 5;

    if ((IsKeyPressed(accept) && menuState == 3) || IsKeyPressed(back)) return MENU;
    
    return OPTIONS;
}

void UI::drawButton(float w, float h, const char* text, int size, Color buttonColor, Color textColor, float divisor, float number, float yOffset) {
    float orient = (number / divisor);
    DrawRectangle(
        (SCREEN_W - w) / 2.f, // Horizontally centered
        (SCREEN_H - h) * orient, // Vertically centered by a factor
        w, // width
        h, // height
        buttonColor
    );
    DrawText(
        text,
        (SCREEN_W - MeasureText(text, size)) / 2.f, // Horizontally centered
        (SCREEN_H - h + yOffset) * orient, // Vertically centered by a factor
        size, // font size
        textColor
    );
}

void UI::drawSelector(float w, float h, float divisor, float number, int thick, Color color) {
    float orient = (number / divisor);
    Rectangle selector = {
        (SCREEN_W - w) / 2.f, // Horizontally centered
        (SCREEN_H - h) * orient, // Vertically centered by fourths
        w, // width
        h, // height
    };
    DrawRectangleLinesEx(
        selector, 
        thick, // Thickness
        color
    );
}

void UI::drawMenu() {
    BeginDrawing();
    
    ClearBackground(BLACK);

    DrawTexturePro(backG, {0, 0, 67, 59}, {(SCREEN_W - 67) / 2.f, 20, 67 * 4, 59 * 4}, {0, 0}, 0.f, WHITE);

    drawButton(200, 100, "Play", 40, WHITE, BLACK, 4.f, 1.f, 120);
    drawButton(200, 100, "Options", 40, WHITE, BLACK, 4.f, 2.f, 60);
    drawButton(200, 100, "Quit", 40, WHITE, BLACK, 4.f, 3.f, 40);
    drawSelector(200, 100, 4.f, menuState, 5, RED);
    
    EndDrawing();
}

void UI::drawOptions() {
    BeginDrawing();
    
    ClearBackground(BLACK);

    drawButton(200, 100, "Volume", 40, WHITE, WHITE, 4.f, 1.f, 120);
    drawButton(200 * volume / 5.f, 100, "Volume", 40, GREEN, BLACK, 4.f, 1.f, 120);
    drawButton(200, 100, "Fullscreen", 30, WHITE, BLACK, 4.f, 2.f, 70);
    drawButton(200, 100, "Back", 40, WHITE, BLACK, 4.f, 3.f, 40);
    drawSelector(200, 100, 4.f, menuState, 5, RED);
    
    EndDrawing();
}