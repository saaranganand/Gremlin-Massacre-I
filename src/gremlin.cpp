#include <../include/raylib.h>
#include <../include/gremlin.h>

Gremlin::Gremlin(float x, float y) : Actor(x, y, 30, 50) {
    anims = AnimationHandler();

    Vector2 dest = {100,100};
    Vector2 offset = {0,-27};
    float fxOff = -78.f;

    createAnimation("player/", "idle", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "run", 4, {32,32}, dest, true, offset, fxOff);
    createAnimation("player/", "fall", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "neutral", 5, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "pogo", 5, {32,32}, dest, false, offset, fxOff);

    anims.setAnim("idle");

    Attack neutralA("neutral", 100.f, 110.f, hurtbox.size.x, -30.f, -100.f, 0.5f, 0.02f);
    Attack pogo("neutral", 90.f, 90.f, -45.f + hurtbox.size.x / 2.f, hurtbox.size.y, 0.f, 0.5f, 0.05f);

    atks = AttackHandler();
    atks.attacks["neutral"] = neutralA;
    atks.attacks["pogo"] = pogo;
    atks.current = &neutralA;

    state = IDLE;

    groundAcceleration = 2000.f;
    groundMaxVelocity = 600.f;

    airAcceleration = 1500.f;
    airMaxVelocity = 400.f;

    aggroRange = 200.f;

    attackRange = 150.f;

    scaredTimer = Timer(4.f);
    scaredRunTimer = Timer(1.f);

    invincibilityTimer = Timer(.5f);

    maxHealth = 3;
    health = 3;
}

void Gremlin::gremlinJump() {
    if (grounded) {
        grounded = false;
        CH_velocity.y = jumpVelocity;
    }
}

void Gremlin::update(Map map, Player& player, std::vector<Coin>& coins, float dt) {
    if (exploded) return;

    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);

    collideWithVerticalStaticStage(map, dt);
    hurtbox.parentPosition.y = position.y;

    float dist = std::abs(player.position.x - position.x);

    if (state == IDLE && dist < aggroRange) {
        if (player.position.x + player.hurtbox.size.x / 2.f < position.x + hurtbox.size.x / 2.f) state = RUNRIGHT;
        else state = RUNLEFT;
    }

    if (state == FIGHT && dist < attackRange && !atks.active) {
        atks.play("neutral");
    }

    if (state == FIGHT && atks.done && atks.active) {
        scaredTimer.start();
        scaredRunTimer.start();
        scaredRunTimer.time = 1.f;

        if (anims.flipX) state = RUNLEFT;
        else state = RUNRIGHT;
    }

    if (scaredTimer.active) {
        if (!scaredRunTimer.active) {
            if (state == RUNLEFT) state = RUNRIGHT;
            else state = RUNLEFT;
            scaredRunTimer.time = ((float) (rand() % 100) + 50.f) / 100.f;
            scaredRunTimer.play();
        }
    } else {
        if (state == RUNRIGHT && sign(position.x - player.position.x) == -1) state = RUNLEFT;
        else if (state == RUNLEFT && sign(position.x - player.position.x) == 1) state = RUNRIGHT;
    }

    int input = 0;
    if (state == RUNLEFT) input = -1;
    else if (state == RUNRIGHT) input = 1;

    handleInput(input, dt);

    collideWithHorizontalStaticStage(map, dt);

    if ((state == RUNLEFT || state == RUNRIGHT) && CH_velocity.x == 0.f) {
        state = FIGHT;
        if (anims.flipX) {
            anims.flipX = false;
            atks.flipX = false;
        } else {
            anims.flipX = true;
            atks.flipX = true;
        }
    }

    hurtbox.parentPosition.x = position.x;

    if (!atks.active && CH_velocity.x != 0.f) {
        if (sign(CH_velocity.x) == 1) {
            anims.flipX = false;
            atks.flipX = false;
        }
        else if (sign(CH_velocity.x) == -1) {
            anims.flipX = true;
            atks.flipX = true;
        }
    }

    invincibilityTimer.update(dt);
    if (player.atks.active && !invincibilityTimer.active) if (hurtbox.collides(player.atks.current->hitbox)) {
        float dirX = 1;
        float dirY = -1;

        if (right() < player.left()) dirX = -1;
        if (top() > player.bottom()) dirY = 1;

        player.KB_velocity.x = -1000.f;
        if (player.atks.flipX) player.KB_velocity.x = 1000.f;
        
        takeDamage(1, 0.f, {dirX,dirY}, coins);
        invincibilityTimer.start();

        
    }

    if (atks.active && atks.current != NULL) anims.current = &anims.animations[atks.current->anim.c_str()];
    else if (CH_velocity.y != 0.f || !grounded) anims.current = &anims.animations["fall"];
    else if (CH_velocity.x != 0.f) anims.current = &anims.animations["run"];
    else anims.current = &anims.animations["idle"];

    atks.update(position.x, position.y, dt);

    anims.update(dt);
    scaredRunTimer.update(dt);
    scaredTimer.update(dt);

}

void Gremlin::explode(std::vector<Coin>& coins) {
    exploded = true;

    for (int i = 0; i < value; i++) {
        Vector2 pos = position;
        Vector2 vel = {(rand() % 10) - 5.f, (rand() % 10) - 5.f};
        Coin c = Coin(pos, vel);
        
        coins.push_back(c);
    }
}

void Gremlin::takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins) {
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

void Gremlin::draw(bool debugging, float dt) {
    if (exploded) return;
    if (debugging) {
        hurtbox.draw();
        atks.current->hitbox.draw();
    }

    Color tint = WHITE;
    if (((int) (invincibilityTimer.elapsed * 10.f) % 2)) tint = DARKGRAY;

    anims.draw(position.x, position.y, tint);
}

void Gremlin::kill() {
    anims.kill();
}