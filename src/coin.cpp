#include<../include/raylib.h>
#include <../include/coin.h>

Coin::Coin(Vector2 pos, Vector2 vel) {
    position = pos;
    velo = {vel.x * 200.f, vel.y * 200.f};

    col = Collider(pos, 0, 0, 20, 20);
    grounded = false;
    grav = 1500.f;
}


void Coin::collideWithVerticalStaticStage(Map map, float dt) {
    // We start from the moving edge (top or bottom).
    float velocity = velo.y * dt;
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
    velo.y *= -0.98;
    if (direction > 0) grounded = true;

    // Move to where the collision occured, with a small collision edge offset.
    position.y += (closest - movingEdge) - COLLISION_EDGE * direction;
    return;
}

void Coin::collideWithHorizontalStaticStage(Map map, float dt) {
    // We start from the moving edge (left or right).
    float velocity = velo.x * dt;
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

    velo.x *= -0.98;

    // Move to where the collision occured, with a small collision edge offset.
    position.x += (closest - movingEdge) - COLLISION_EDGE * direction;

    return;
}

bool Coin::update(Player& player, Map map, float dt) {
    velo.y += grav * dt;

    collideWithVerticalStaticStage(map, dt);
    collideWithHorizontalStaticStage(map, dt);

    col.update(position);

    if (col.collides(player.hurtbox)) {
        player.coins++;
        return true;
    }
    return false;
}

void Coin::draw(Texture2D tex) {
    DrawTexturePro(tex, {0, 0, 20, 20}, {position.x, position.y, 20, 20}, ZERO, 0.f, WHITE);
}

float Coin::left() {
    return position.x;
}

float Coin::right() {
    return position.x + col.size.x;
}

float Coin::top() {
    return position.y;
}

float Coin::bottom() {
    return position.y + col.size.y;
}