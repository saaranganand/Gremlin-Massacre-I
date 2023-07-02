#include <../include/raylib.h>
#include <../include/player.h>

#include <vector>
#include <math.h>
#include <stdio.h>

void Player::loadAnimationsAtacks() {
    anims = AnimationHandler();
    
    Vector2 dest = {120,120};
    Vector2 offset = {0,-27};
    float fxOff = -78.f;

    createAnimation("player/", "idle", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "run", 4, {32,32}, dest, true, offset, fxOff);
    createAnimation("player/", "fall", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "neutral", 5, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "pogo", 5, {32,32}, dest, false, offset, fxOff);

    anims.setAnim("idle");

    Attack neutral("neutral", 90.f, 50.f, hurtbox.size.x, 20.f, -100.f, 0.5f, 0.02f);
    Attack pogo("pogo", 90.f, 90.f, -45.f + hurtbox.size.x / 2.f, hurtbox.size.y, -45.f / 2.f, 0.5f, 0.05f);

    atks = AttackHandler();
    atks.attacks["neutral"] = neutral;
    atks.attacks["pogo"] = pogo;
    atks.current = &neutral;
}

Player::Player(float x, float y) : Actor(x, y, 40, 80) {
    anims = AnimationHandler();

    loadAnimationsAtacks();

    earlyJumpTimer = Timer(.1f);
    lateJumpTimer = Timer(.1f);

    swingKB = 350.f;
    spikeKB = 700.f;
    pogoKB = 700.f;

    pogoDeAccel = 700.f;
    invincibilityTimer = Timer(2.f);

    coins = 0;

    maxEstus = 0;
    estus = 0;

    hurt = LoadSound("assets/player/hurt.wav");
    attack = LoadSound("assets/player/attack.wav");
    hitWall = LoadSound("assets/player/hitWall.wav");
    coin = LoadSound("assets/player/coin.wav");
}

void Player::handleJump(KeyboardKey jumpKey, float dt) {
    // Allow the player to jump before they touch the ground or after they leave the ground by a small margin.
    if (IsKeyDown(jumpKey)) earlyJumpTimer.start();
    else earlyJumpTimer.kill();

    // Trigger jump.
    if (earlyJumpTimer.active && (grounded || lateJumpTimer.active)) {
        grounded = false;
        CH_velocity.y = jumpVelocity;

        earlyJumpTimer.kill();
        lateJumpTimer.kill();
    }
    // Decrease jump height if key let go during jump ascension.
    if (!IsKeyDown(jumpKey) && CH_velocity.y < 0.f) applyGravity(dt);

    // Update early and late jump timers.
    earlyJumpTimer.update(dt);
    lateJumpTimer.update(dt);
}

bool Player::handleMapDamage(Map map) {
    // Loop through all cells that player currently lies in.
    int leftColumn = left() / map.tileSize;
    int rightCloumn = right() / map.tileSize;

    int topRow = top() / map.tileSize;
    int bottomRow = bottom() / map.tileSize;

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == DAMAGE) {
                // Take damage and knockback, and set off invincibility timer.
                float dirX = 1;
                if (right() < x * map.tileSize + map.tileSize / 2.f) dirX = -1;

                float dirY = 1;
                if (bottom() < y * map.tileSize + map.tileSize / 2.f) dirY = -1;

                takeDamage(map.getTile(x, y)->damage , 1200.f, {dirX, dirY});
                invincibilityTimer.start();
                return true;
            }
        }
    }
    return false;
}

void Player::takeDamage(int damage, float knockback, Vector2 KB_dir) {
    PlaySound(hurt);
    health -= damage;
    if (health <= 0) {
        health = 0;
        KB_velocity.x = 1300.f * KB_dir.x;
        KB_velocity.y = 1300.f * KB_dir.y;
    } else {
        KB_velocity.x = knockback * KB_dir.x;
        KB_velocity.y = knockback * KB_dir.y;
    }
}

bool Player::checkBonfire(Map map) {
    // Loop through all cells that player currently lies in.
    int leftColumn = left() / map.tileSize;
    int rightCloumn = right() / map.tileSize;

    int topRow = top() / map.tileSize;
    int bottomRow = bottom() / map.tileSize;

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == BONFIRE) {
                //play healing sound
                health = maxHealth;
                estus = maxEstus;
                return true;
            }
        }
    }
    return false;
}

void Player::update(Map map, UI ui, float dt) {
    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);

    handleJump(ui.jump, dt);
    collideWithVerticalStaticStage(map, dt);

    hurtbox.parentPosition.y = position.y;
    
    int input = 0;
    if (IsKeyDown(ui.left)) input--;
    if (IsKeyDown(ui.right)) input++;
    handleInput(input, dt);

    //if (input != 0) PlaySound(walk);

    collideWithHorizontalStaticStage(map, dt);

    hurtbox.parentPosition.x = position.x;

    invincibilityTimer.update(dt);
    if (!invincibilityTimer.active && handleMapDamage(map)) invincibilityTimer.start();

    // Take damage
    // Handle death

    if (IsKeyPressed(ui.heal)) {
        checkBonfire(map);
    }

    if (IsKeyPressed(ui.action)) {
        if (IsKeyDown(ui.down) && !grounded) atks.play("pogo");
        else atks.play("neutral");
        PlaySound(attack);
    }
    atks.update(position.x, position.y, dt);

    if (atks.active && atks.current == &atks.attacks["neutral"] && atks.attackTime / dt < 2) {
        if (hitStage(map, atks.current->hitbox, STATIC)) {
            KB_velocity.x = -1000.f;
            if (atks.flipX) KB_velocity.x = 1000.f;
            PlaySound(hitWall);
        }
    }

    if (atks.active && atks.current == &atks.attacks["pogo"]) {
        if (hitStage(map, atks.current->hitbox, DAMAGE)) {
            CH_velocity.y = jumpVelocity;
        }
    }

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

    if (grounded && atks.current == &atks.attacks["pogo"] && atks.active) {
        atks.active = false;
    }

    if (IsKeyPressed(ui.heal) && estus > 0) {
        estus--;
        health++;
    }

    if (atks.active && atks.current != NULL) anims.current = &anims.animations[atks.current->anim.c_str()];
    else if (CH_velocity.y != 0.f || !grounded) anims.current = &anims.animations["fall"];
    else if (CH_velocity.x != 0.f) anims.current = &anims.animations["run"];
    else anims.current = &anims.animations["idle"];

    anims.update(dt);
}

void Player::draw(bool debugging, float dt) {
    if (debugging) {
        hurtbox.draw();
        atks.current->hitbox.draw();
    }
    
    Color tint = WHITE;
    if (((int) (invincibilityTimer.elapsed * 10.f) % 2)) tint = DARKGRAY;

    anims.draw(position.x, position.y, tint);
}

void Player::drawHP() {

    Rectangle rec = { 30, 30, 30, 30};

    for (int i = 0; i < maxHealth; i++) {
        Color color = GRAY;
        if (health > i) color = RED;

        DrawRectangleRec(rec, color);
        DrawRectangleLinesEx(rec, 5, BLACK);
        rec.x += 40;
    }

    DrawText(TextFormat("$ = %d", coins), 30, 80, 40, WHITE);
    DrawText(TextFormat("E = %d", estus), 30, 150, 40, WHITE);
}

void Player::kill() {
    anims.kill();
    UnloadSound(hurt);
    UnloadSound(attack);
    UnloadSound(hitWall);
    UnloadSound(coin);
}