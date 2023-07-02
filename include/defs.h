#ifndef DEFS_H_
#define DEFS_H_

#include <raylib.h>
#include <string>

extern int SCREEN_W;
extern int SCREEN_H;;

#define ZERO { 0.f, 0.f }

enum State { MENU = 0, PLAY, OPTIONS, YOUDIED, SHOPPING, NOTICE, WIZARDING, QUIT };

Vector2 convertTopLeft(Vector2 vector, Vector2 size);
Rectangle convertTopLeft(Rectangle rectangle);

int sign(float a);
int sign(int a);

void clamp(float& a, float min, float max);
void clamp(int& a, int min, int max);

void deaccelerateToZero(float& velocity, float deacceleration, float dt);

Texture2D loadTextureUnloadImage(std::string );

#endif