#include <../include/actor.h>

#include <algorithm>
#include <vector>
#include <stdio.h>

Actor::Actor(float x, float y, float w, float h) {
    position = { x, y };

    hurtbox = Collider(position, 0.f, 0.f, w, h);

    alive = true;
    maxHealth = 3;
    health = maxHealth;

    CH_velocity = ZERO;

    groundAcceleration = 1800.f;
    groundMaxVelocity = 500.f;

    airAcceleration = 1500.f;
    airMaxVelocity = 350.f;

    xDeacceleration = 4000.f;

    jumpVelocity = -650.f;
    grounded = false;

    KB_velocity = ZERO;
    exploded = false;

    value = 10;
}

void Actor::createAnimation(std::string dir, std::string tag, int frames, Vector2 source, Vector2 dest, bool loop, Vector2 offset, float flipOffset, int fps) {
    std::string startDir = "assets/";
    std::string extension = ".png";
    
    Texture tex = loadTextureUnloadImage(startDir + dir + tag + extension);
    Animation anim = Animation(frames, source.x, source.y, dest.x, dest.y, loop, flipOffset, offset.x, offset.y, fps);
    
    anim.sheet = tex;
    anims.animations[tag] = anim;
}

void Actor::deaccelerateKnockback(float dt) {
    deaccelerateToZero(KB_velocity.x, KNOCKBACK_DEACCELERATION, dt);
    deaccelerateToZero(KB_velocity.y, KNOCKBACK_DEACCELERATION, dt);
}

void Actor::applyGravity(float dt) {
    // Pick the minimum between (velocity + acceleration) or max velocity.
    CH_velocity.y = std::min(CH_velocity.y + GRAVITY_ACCELERATION * dt, MAX_GRAVITY_VELOCITY);
}

void Actor::checkIfNotGrounded(float dt) {
    if (abs(CH_velocity.y) > NOT_GROUNDED_THRESHOLD * dt) grounded = false;
}

void Actor::collideWithVerticalStaticStage(Map map, float dt) {
    // We start from the moving edge (top or bottom).
    float velocity = (CH_velocity.y + KB_velocity.y) * dt;
    int direction = sign(velocity);

    float movingEdge = bottom();
    if (direction == -1) movingEdge = top();

    // And look for tiles in between our target positon, and our actual position.
    int start = movingEdge / map.tileSize;
    int limit = (movingEdge + velocity) / map.tileSize;

    // Clamp map searching to bounds of the array
    clamp(start, 0, map.height - 1);
    clamp(limit, 0, map.height - 1);

    std::vector<float> collisions;

    // We only need to look for vertical collisions along the columns that the player resides in.
    int leftEdge = left() / map.tileSize;
    int rightEdge = right() / map.tileSize;

    clamp(leftEdge, 0, map.width - 1);
    clamp(rightEdge, 0, map.width - 1);

    for (; leftEdge <= rightEdge; leftEdge++) {

        // We look along the path of the actor for any static objects.
        for (int cell = start; cell != (limit + direction); cell += direction) {
            if (map.getTile(leftEdge, cell)->type == STATIC) {

                // Find the real position of the tile
                float collisionEdge = cell * map.tileSize;
                if (direction == -1) collisionEdge += map.tileSize;

                // Record the distance between the moving edge and the collision edge
                collisions.push_back(collisionEdge);
                break; // We can break because we only care about the closest
            }
        }
    }

    // No collisions nearby, just move.
    if (collisions.size() == 0) {
        position.y += velocity;
        return;
    }

    // Find the closest collision.
    float closest = collisions[0];
    for (unsigned int i = 1; i < collisions.size(); i++) {
            if (direction == 1) closest = std::min(closest, collisions[i]);
            else closest = std::max(closest, collisions[i]);
        }

    // If no collisions occur, just move.
    if (std::abs(closest - movingEdge) > std::abs(velocity)) {
        position.y += velocity;
        return;
    }

    // Reset movement velocity and check if actor is grounded.
    CH_velocity.y = 0.f;
    if (direction > 0) grounded = true;

    // Knockback should bounce actor off the wall.
    KB_velocity.y *= -1;

    // Move to where the collision occured, with a small collision edge offset.
    position.y += (closest - movingEdge) - COLLISION_EDGE * direction;
    return;
}

void Actor::handleInput(int input, float dt) {
    // Deaccelerate if not moving.
    
    if (input == 0) deaccelerateToZero(CH_velocity.x, xDeacceleration, dt);
    else {
        // Stop movement if we switch directions.
        if (input != sign(CH_velocity.x)) CH_velocity.x = 0.f;

        // Apply acceleration based on whether actor is grounded.
        if (grounded) CH_velocity.x += groundAcceleration * input * dt;
        else CH_velocity.x += airAcceleration * input * dt;
    }

    // Clamp movement velocity based on whether actor is grounded.
    if (grounded) clamp(CH_velocity.x, -groundMaxVelocity, groundMaxVelocity);
    else clamp(CH_velocity.x, -airMaxVelocity, airMaxVelocity);
}

void Actor::collideWithHorizontalStaticStage(Map map, float dt) {
    // We start from the moving edge (left or right).
    float velocity = (CH_velocity.x + KB_velocity.x) * dt;
    int direction = sign(velocity);
    
    float movingEdge = right();
    if (direction == -1) movingEdge = left();

    // And look for tiles in between our target positon, and our actual position.
    int start = movingEdge / map.tileSize;
    int limit = (movingEdge + velocity) / map.tileSize;

    // Clamp map searching to bounds of the array
    clamp(start, 0, map.width - 1);
    clamp(limit, 0, map.width - 1);
    
    std::vector<float> collisions;

    // We only need to look for vertical collisions along the rows that the player resides in.
    int topEdge = top() / map.tileSize;
    int BottomEdge = bottom() / map.tileSize;

    clamp(topEdge, 0, map.height - 1);
    clamp(BottomEdge, 0, map.height - 1);

    for (; topEdge <= BottomEdge; topEdge++) {

        // We look along the path of the actor for any static objects.
        for (int cell = start; cell != (limit + direction); cell += direction) {
            if (map.getTile(cell, topEdge)->type == STATIC) {
                
                // Find the real position of the tile
                float collisionEdge = cell * map.tileSize;
                if (direction == -1) collisionEdge += map.tileSize;

                // Record the distance between the moving edge and the collision edge
                collisions.push_back(collisionEdge);
                break; // We can break because we only care about the closest
            }
        }
    }

    // No collisions nearby, just move.
    if (collisions.size() == 0) {
        position.x += velocity;
        return;
    }

    // Find the closest collision.
    float closest = collisions[0];
    for (unsigned int i = 1; i < collisions.size(); i++) {
        if (direction == 1) closest = std::min(closest, collisions[i]);
        else closest = std::max(closest, collisions[i]);
    }

    // If no collisions occur, just move.
    if (std::abs(closest - movingEdge) > std::abs(velocity)) {
        position.x += velocity;
        return;
    }

    // Reset movement velocity.
    CH_velocity.x = 0.f;

    // Knockback should bounce actor off the wall.
    KB_velocity.x *= -1;

    // Move to where the collision occured, with a small collision edge offset.
    position.x += (closest - movingEdge) - COLLISION_EDGE * direction;

    return;
}

void Actor::takeDamage(int damage, float knockback, Vector2 KB_dir) {
    health -= damage;
    if (health <= 0) {
        explode();
        health = 0;
        KB_velocity.x = 1300.f * KB_dir.x;
        KB_velocity.y = 1300.f * KB_dir.y;
    } else {
        KB_velocity.x = knockback * KB_dir.x;
        KB_velocity.y = knockback * KB_dir.y;
    }
}

bool Actor::hitStage(Map map, Collider box, TileType tType) {
    Vector2 pos = {box.parentPosition.x + box.offset.x,box.parentPosition.y + box.offset.y};
    // Loop through all cells that player currently lies in.
    int leftColumn = pos.x / map.tileSize;
    int rightCloumn = (pos.x + box.size.x) / map.tileSize;

    int topRow = pos.y / map.tileSize;
    int bottomRow = (pos.y + box.size.y) / map.tileSize;

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == tType) {
                return true;
            }
        }
    }
    return false;
}

void Actor::end() {
    alive = false;
}

void Actor::explode() {
    exploded = true;
}

float Actor::left() {
    return position.x;
}

float Actor::right() {
    return position.x + hurtbox.size.x;
}

float Actor::top() {
    return position.y;
}

float Actor::bottom() {
    return position.y + hurtbox.size.y;
}