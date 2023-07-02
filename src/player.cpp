#include <../include/raylib.h>
#include <../include/player.h>

#include <vector>
#include <math.h>
#include <stdio.h>

void Player::resetDeath() {
    health = maxHealth;
    estus = maxEstus;

    CH_velocity.x = 0.f;
    CH_velocity.y = 0.f;

    KB_velocity.x = 0.f;
    KB_velocity.y = 0.f;

    coins -= 10;
}

void Player::loadAnimationsAtacks() {
    anims = AnimationHandler();
    
    Vector2 dest = {120,120};
    Vector2 offset = {0,-27};
    float fxOff = -78.f;

    createAnimation("player/", "idle", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "run", 4, {32,32}, dest, true, offset, fxOff);
    createAnimation("player/", "fall", 1, {32,32}, dest, false, offset, fxOff);
    createAnimation("player/", "neutral", 5, {32,32}, dest, false, offset, fxOff);
    dest.y *= 2;
    createAnimation("player/", "pogo", 5, {32,64}, dest, false, offset, fxOff);

    anims.setAnim("idle");

    Attack neutral("neutral", 90.f, 50.f, hurtbox.size.x, 20.f, -100.f, 0.5f, 0.02f);
    Attack pogo("pogo", 90.f, 120.f, -45.f + hurtbox.size.x / 2.f, hurtbox.size.y, -45.f / 2.f, 0.5f, 0.05f);

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

    jumpVelocity = -700.f;

    pogoDeAccel = 700.f;
    invincibilityTimer = Timer(2.f);

    coins = 0;

    maxEstus = 0;
    estus = maxEstus;

    hurt = LoadSound("assets/player/hurt.wav");
    attack = LoadSound("assets/player/attack.wav");
    hitWall = LoadSound("assets/player/hitWall.wav");
    coin = LoadSound("assets/player/coin.wav");
    bonfire = LoadSound("assets/player/bonfire.mp3");
}

void Player::handleJump(KeyboardKey jumpKey, Map map, float dt) {
    // Allow the player to jump before they touch the ground or after they leave the ground by a small margin.
    if (IsKeyDown(jumpKey)) earlyJumpTimer.start();
    else earlyJumpTimer.kill();

    // Trigger jump.
    if (earlyJumpTimer.active && ((grounded || lateJumpTimer.active) || checkLadder(map))) {
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

    clamp(leftColumn, 0, map.width - 1);
    clamp(rightCloumn, 0, map.width - 1);
    clamp(topRow, 0, map.height - 1);
    clamp(bottomRow, 0, map.height - 1);

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

bool Player::checkShop(Map map) {
    // Loop through all cells that player currently lies in.
    int leftColumn = left() / map.tileSize;
    int rightCloumn = right() / map.tileSize;

    int topRow = top() / map.tileSize;
    int bottomRow = bottom() / map.tileSize;

    clamp(leftColumn, 0, map.width - 1);
    clamp(rightCloumn, 0, map.width - 1);
    clamp(topRow, 0, map.height - 1);
    clamp(bottomRow, 0, map.height - 1);

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == SHOP) {
                //play healing sound
                return true;
            }
        }
    }
    return false;
}

bool Player::checkBonfire(Map map) {
    // Loop through all cells that player currently lies in.
    int leftColumn = left() / map.tileSize;
    int rightCloumn = right() / map.tileSize;

    int topRow = top() / map.tileSize;
    int bottomRow = bottom() / map.tileSize;

    clamp(leftColumn, 0, map.width - 1);
    clamp(rightCloumn, 0, map.width - 1);
    clamp(topRow, 0, map.height - 1);
    clamp(bottomRow, 0, map.height - 1);

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == BONFIRE) {
                PlaySound(bonfire);
                health = maxHealth;
                estus = maxEstus;
                if (!bonfires.count(map.mapName)) {
                    bonfires[map.mapName] = {map.bonfireCoords.x, map.bonfireCoords.y, map.width, map.height};
                }
                currentBonfire = map.mapName;
                return true;
            }
        }
    }
    return false;
}

bool Player::checkLadder(Map map) {
    // Loop through all cells that player currently lies in.
    int leftColumn = left() / map.tileSize;
    int rightCloumn = right() / map.tileSize;

    int topRow = top() / map.tileSize;
    int bottomRow = bottom() / map.tileSize;

    clamp(leftColumn, 0, map.width - 1);
    clamp(rightCloumn, 0, map.width - 1);
    clamp(topRow, 0, map.height - 1);
    clamp(bottomRow, 0, map.height - 1);

    for (int x = leftColumn; x <= rightCloumn; x++) {
        for (int y = topRow; y <= bottomRow; y++) {
            if (map.getTile(x, y)->type == LADDER) {
                return true;
            }
        }
    }
    return false;
}

bool Player::update(Map map, UI ui, float dt) {
    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);
    handleJump(ui.jump, map, dt);
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
        if (checkBonfire(map)) {
            health = maxHealth;
            estus = maxEstus;

            save();
        }
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
    if (IsKeyPressed(ui.heal)) {
        if (checkShop(map)) {
            return true;
        } else if (estus > 0 && !checkBonfire(map) && health < maxHealth) {
            estus--;
            health++;
        }
    }
    
    
    if (atks.active && atks.current != NULL) anims.current = &anims.animations[atks.current->anim.c_str()];
    else if (CH_velocity.y != 0.f || !grounded) anims.current = &anims.animations["fall"];
    else if (CH_velocity.x != 0.f) anims.current = &anims.animations["run"];
    else anims.current = &anims.animations["idle"];
    anims.update(dt);
    return false;
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
    UnloadSound(bonfire);
}

bool Player::load() {
    std::ifstream saveData("player.save");
    if (!saveData) return false;

    std::string line;
    getline(saveData, line);
    maxEstus = stoi(line);

    getline(saveData, line);
    coins = stoi(line);
    printf("%d %d\n", coins, stoi(line));

    if (getline(saveData, line)) {
        if (line != "") {
            currentBonfire = line;
        } else return false;
        
    } else {
        return false;
    }
    

    while (saveData) {
        // Read each line of the save data.
        std::string line;
        if (!getline(saveData, line)) break;

        std::istringstream dataStream(line);
        levelData d = { 0, 0, 0, 0};
        std::string name;
        int i = 0;
        while (dataStream) {
            
            std::string val;

            

            // Split map data into comma seperated list.
            if (!getline(dataStream, val, ',')) break;
            if (i == 0) name = val;
            if (i == 1) d.bonfireX = stoi(val);
            if (i == 2) d.bonfireY = stoi(val);
            if (i == 3) d.width = stoi(val);
            if (i == 4) d.height = stoi(val);
            
            i++;
        }
        if (i == 5) {

            bonfires[name] = d;

        }
    }

    return true;
}

void Player::save() {
    std::ofstream Saved("player.save");

    Saved << maxEstus << "\n";
    Saved << coins << "\n";
    Saved << currentBonfire << "\n";
    std::map<std::string, levelData>::iterator it;

    for (it = bonfires.begin(); it != bonfires.end(); it++) {
        Saved << it->first << ",";
        Saved << it->second.bonfireX << ",";
        Saved << it->second.bonfireY << ",";
        Saved << it->second.width << ",";
        Saved << it->second.height << "\n";
    }

    Saved.close();
}