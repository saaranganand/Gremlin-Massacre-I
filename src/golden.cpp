#include <../include/raylib.h>
#include <../include/golden.h>

Golden::Golden(float x, float y) : Actor (x, y, 32, 32) {
    anims = AnimationHandler();

    Vector2 dest = {32,32};
    Vector2 offset = {0, 0};
    float fxOff = 0.f;

    createAnimation("golden/", "idle", 1, {8,8}, dest, true, offset, fxOff);
    anims.setAnim("idle");

    groundAcceleration = 1500.f;
    groundMaxVelocity = 400.f;

    airAcceleration = 0.f;
    
    maxHealth = 10;
    health = 10;

    value = 50;

    invincibilityTimer = Timer(.5f);

    hurt = LoadSound("assets/golden/hurt.wav");
    explosion = LoadSound("assets/golden/explosion.wav");
}

void Golden::explode(std::vector<Coin>& coins) {
    exploded = true;
    //PlaySound(action);
    PlaySound(explosion);

    for (int i = 0; i < value; i++) {
        Vector2 pos = position;
        Vector2 vel = {(rand() % 10) - 5.f, (rand() % 10) - 5.f};
        Coin c = Coin(pos, vel);
        
        coins.push_back(c);
    }
}

void Golden::takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins) {
    PlaySound(hurt);
    health -= damage;
    if (health <= 0) {
        explode(coins);
        health = 0;
        KB_velocity.x = 1300.f * KB_dir.x;
        KB_velocity.y = 1300.f * KB_dir.y;
    } else {
        KB_velocity.x = knockback * KB_dir.x;
        KB_velocity.y = knockback * KB_dir.y;
    }
}


void Golden::update(Map map, Player& player, std::vector<Coin>& coins, float dt) {
    if (exploded) return;

    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);

    collideWithVerticalStaticStage(map, dt);
    hurtbox.parentPosition.y = position.y;

    int input = 0;

    if (position.x > player.position.x) input = -1;
    else input = 1;


    handleInput(input, dt);

    //if (input != 0) PlaySound(walk);

    collideWithHorizontalStaticStage(map, dt);
    
    hurtbox.parentPosition.x = position.x;

    if (player.atks.active && !invincibilityTimer.active) if (hurtbox.collides(player.atks.current->hitbox)) {
        float dirX = 1;
        float dirY = -1;

        if (right() < player.left()) dirX = -1;
        if (top() > player.bottom()) dirY = 1;

        player.KB_velocity.x = -1000.f;
        if (player.atks.flipX) player.KB_velocity.x = 1000.f;
        
        takeDamage(1, 0.f, {dirX,dirY}, coins);
        invincibilityTimer.start();

        if (player.atks.current == &player.atks.attacks["pogo"]) {
            player.CH_velocity.y = player.jumpVelocity;
        }
    }
    invincibilityTimer.update(dt);

    anims.update(dt);
    
}
void Golden::draw(bool debugging, float dt) {
    if (exploded) return;
    if (debugging) {
        hurtbox.draw();
    }
    anims.draw(position.x, position.y, WHITE);
}

void Golden::kill() {
    anims.kill();
    UnloadSound(hurt);
    UnloadSound(explosion);
}