#include <../include/raylib.h>
#include <../include/frog.h>

Frog::Frog(float x, float y) : Actor(x, y, 32, 32) {
    anims = AnimationHandler();

    Vector2 dest = {32,32};
    Vector2 offset = {0, 0};
    float fxOff = 0.f;

    createAnimation("frog/", "jump", 3, {16,16}, dest, true, offset, fxOff);
    anims.setAnim("jump");

    hitbox = Collider(position, 0.f, 0.f, 32, 32);

    groundAcceleration = 0.f;
    groundMaxVelocity = 0.f;

    airAcceleration = 300.f;
    airMaxVelocity = 900.f;

    aggroRange = 350.f;
    aggroed = false;
    
    maxHealth = 1;
    health = 1;

    value = 3;

    action = LoadSound("assets/frog/action.mp3");
    hurt = LoadSound("assets/frog/hurt.wav");
    explosion = LoadSound("assets/frog/explosion.wav");
};

void Frog::frogJump() {
    if (grounded) {
        CH_velocity.y = -1000.f;
        grounded = false;
    }
}

void Frog::explode(std::vector<Coin>& coins) {
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
void Frog::takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins) {
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

void Frog::update(Map map, Player& player, std::vector<Coin>& coins, float dt) {
    if (exploded) return;

    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);

    frogJump();

    collideWithVerticalStaticStage(map, dt);
    hurtbox.parentPosition.y = position.y;
    hitbox.parentPosition.y = position.y;

    float dist = std::abs(player.position.x - position.x);
    if (dist < aggroRange) aggroed = true;

    int input = 0;
    if (aggroed) {
        if (position.x > player.position.x) input = -1;
        else input = 1;
    }

    handleInput(input, dt);

    //if (input != 0) PlaySound(walk);

    collideWithHorizontalStaticStage(map, dt);
    
    hurtbox.parentPosition.x = position.x;
    hitbox.parentPosition.x = position.x;

    if (player.atks.active) if (hurtbox.collides(player.atks.current->hitbox)) {
        float dirX = 1;
        float dirY = -1;

        if (right() < player.left()) dirX = -1;
        if (top() > player.bottom()) dirY = 1;

        player.KB_velocity.x = -1000.f;
        if (player.atks.flipX) player.KB_velocity.x = 1000.f;
        
        takeDamage(1, 0.f, {dirX,dirY}, coins);

        if (player.atks.current == &player.atks.attacks["pogo"]) {
            player.CH_velocity.y = player.jumpVelocity;
        }
    }
    
    if (hitbox.collides(player.hurtbox)) {
        if (!player.invincibilityTimer.active) {
            float dirX = 1;
            float dirY = -1;

            if (right() < player.left()) dirX = -1;
            if (top() > player.bottom()) dirY = 1;

            player.KB_velocity.x = -1000.f;
            if (player.atks.flipX) player.KB_velocity.x = 1000.f;

            player.takeDamage(1, 700.f, {-dirX, dirY});
            player.invincibilityTimer.start();
        }
    }

    anims.update(dt);
    
}
void Frog::draw(bool debugging, float dt) {
    if (exploded) return;
    if (debugging) {
        hurtbox.draw();
    }
    anims.draw(position.x, position.y, WHITE);
}

void Frog::kill() {
    anims.kill();
    UnloadSound(action);
    UnloadSound(hurt);
    UnloadSound(explosion);
}