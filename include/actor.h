#ifndef ACTOR_H_
#define ACTOR_H_

#include <raylib.h>
#include <defs.h>
#include <collider.h>
#include <map.h>

#define COLLISION_EDGE 0.01f

#define GRAVITY_ACCELERATION 1500.f
#define MAX_GRAVITY_VELOCITY 1200.f
#define NOT_GROUNDED_THRESHOLD 1600.f

#define KNOCKBACK_DEACCELERATION 1800.f

struct Actor {
    Vector2 position;

    Collider hurtbox;

    bool alive;
    int maxHealth;
    int health;

    Vector2 CH_velocity;

    float groundAcceleration;
    float groundMaxVelocity;

    float airAcceleration;
    float airMaxVelocity;

    float xDeacceleration;

    float jumpVelocity;
    bool grounded;

    Vector2 KB_velocity;

    Actor(float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f);

    void deaccelerateKnockback(float dt);

    void applyGravity(float dt);
    void checkIfNotGrounded(float dt);

    void collideWithVerticalStaticStage(Map map, float dt);

    void handleInput(int input, float dt);
    void collideWithHorizontalStaticStage(Map map, float dt);

    float left();
    float right();
    float top();
    float bottom();
};

#endif