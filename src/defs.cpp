#include <../include/raylib.h>
#include <../include/defs.h>

#include <algorithm>

Vector2 convertTopLeft(Vector2 vector, Vector2 size) {
    return (Vector2) { vector.x - size.x / 2.f, vector.y - size.y / 2.f };
}

Rectangle convertTopLeft(Rectangle rectangle) {
    Rectangle ret = rectangle;
    ret.x = rectangle.x - rectangle.width / 2.f;
    ret.y = rectangle.y - rectangle.height / 2.f;
    return ret;
}

int sign(float a) {
    int ret = 1;
    if (a < 0.f) ret = -1;

    return ret;
}

int sign(int a) {
    int ret = 1;
    if (a < 0) ret = -1;

    return ret;
}

void clamp(float& a, float min, float max) {
    a = std::max(std::min(a, max), min);
}

void clamp(int& a, int min, int max) {
    a = std::max(std::min(a, max), min);
}

void deaccelerateToZero(float& velocity, float deacceleration, float dt) {
    int velocityDirection = sign(velocity);
    
    if (velocity != 0.f) velocity += deacceleration * -velocityDirection * dt;
    if (sign(velocity) != velocityDirection) velocity = 0.f;
}

Texture2D loadTextureUnloadImage(std::string path) {
    Image img = LoadImage(path.c_str());
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}