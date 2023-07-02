#ifndef UI_H_
#define UI_H_

#include <raylib.h>
#include <defs.h>

struct UI {
    KeyboardKey accept;
    KeyboardKey back;

    KeyboardKey pause;

    KeyboardKey up;
    KeyboardKey down;
    KeyboardKey left;
    KeyboardKey right;

    KeyboardKey action;
    KeyboardKey jump;
    KeyboardKey heal;
    KeyboardKey dash;
    Texture2D backG;

    int menuState;
    int menuMax;

    int volume;

    bool fullscreen;

    UI();

    State menuing();

    void fullscreenTrigger(float& camZoom);
    State optioning(float& camZoom, float dt);

    void drawButton(float w, float h, const char* text, int size, Color buttonColor, Color textColor, float divisor, float number, float yOffset);
    void drawSelector(float w, float h, float divisor, float number, int thick, Color color);

    void drawMenu();
    void drawOptions();
};

#endif
