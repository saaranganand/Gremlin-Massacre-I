#ifndef COLLIDER_H_
#define COLLIDER_H_

#include <raylib.h>
#include <defs.h>

struct Collider {
    Vector2 parentPosition;
    Vector2 offset;
    Vector2 size;
    Color color;

    bool active;
    
    Collider(Vector2 parentPos = ZERO, float x = 0.f, float y = 0.f, int w = 0.f, int h = 0.f);
    void update(Vector2 parentPos);
    void draw();
};

#endif