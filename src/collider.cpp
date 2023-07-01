#include <../include/raylib.h>
#include <../include/collider.h>
#include <../include/defs.h>

#include <stdio.h>

Collider::Collider(Vector2 parentPos, float x, float y, int w, int h) {
    parentPosition = parentPos;
    offset = { x, y };
    size = { (float) w, (float) h };
    color = RED;

    active = true;
}

void Collider::update(Vector2 parentPos) {
    parentPosition = parentPos;
}

void Collider::draw() {
    Rectangle box = { parentPosition.x + offset.x, parentPosition.y + offset.y, size.x, size.y };

    Color drawC = color;
    if (!active) drawC = GRAY;

    DrawRectangleLinesEx(box, 3, drawC);
}