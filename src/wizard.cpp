#include <../include/raylib.h>
#include <../include/wizard.h>

void Wizard::castOrb(Vector2 dir) {
    PlaySound(cast);
    anims.setAnim("cast");\
    orbattack = { { position.x, position.y }, { dir.x * 300, dir.y * 300}};
}

void Wizard::castRay(Vector2 dir) {
    PlaySound(cast);
    anims.setAnim("cast");
    beamAttack = { { position.x, position.y }, { dir.x * 1200, dir.y * 1200} };
}

void Wizard::reset(float x, float y) {
    position.x = x;
    position.y = y;

    state = FOLLOW;

    health = maxHealth;
}

Wizard::Wizard(float x, float y) : Actor(x, y, 200, 200) {
    anims = AnimationHandler();

    Vector2 dest = {200,200};
    Vector2 offset = {0,-40};
    float fxOff = -65.f;

    createAnimation("gandalf/", "idle", 1, {48,70}, dest, true, offset, fxOff);
    createAnimation("gandalf/", "cast", 4, {68,70}, dest, true, offset, fxOff);

    anims.setAnim("idle");

    state = FOLLOW;

    active = false;

    orbattack = { { -10, -10 }, { 0, 0}};
    beamAttack = { { -10, -10 }, { 0, 0}};

    orb = loadTextureUnloadImage("assets/gandalf/orb.png");
    beam = loadTextureUnloadImage("assets/gandalf/lazer.png");

    groundAcceleration = 2000.f;
    groundMaxVelocity = 600.f;

    airAcceleration = 2000.f;
    airMaxVelocity = 700.f;

    followRange = 130.f;

    value = 1000.f;

    invincibilityTimer = Timer(0.5f);
    attackTimer = Timer(1.5f);
    followTimer = Timer(1.f);
    followTimer.start();

    maxHealth = 30;
    health = 30;

    hurt = LoadSound("assets/gandalf/hurt.wav");
    explosion = LoadSound("assets/gandalf/explosion.wav");
    cast = LoadSound("assets/gandalf/laugh.mp3");
}

void Wizard::explode(std::vector<Coin>& coins) {
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
void Wizard::takeDamage(int damage, float knockback, Vector2 KB_dir, std::vector<Coin>& coins) {
    PlaySound(hurt);
    health -= damage;
    if (health <= 0) {
        explode(coins);
        health = 0;
        KB_velocity.x = 0.f * KB_dir.x;
        KB_velocity.y = 0.f * KB_dir.y;
    } else {
        KB_velocity.x = 0 * KB_dir.x;
        KB_velocity.y = 0 * KB_dir.y;
    }
}

void Wizard::update(Map map, Player& player, std::vector<Coin>& coins, float dt) {
    if (!active) return;
    if (state == FOLLOW) {
        followTimer.update(dt);
        if (position.x < player.position.x) {
            CH_velocity.x += airAcceleration * dt;
            if (CH_velocity.x > airMaxVelocity) CH_velocity.x = airMaxVelocity;
        } else if (position.x >= player.position.x) {
            CH_velocity.x -= airAcceleration * dt;
            if (CH_velocity.x < -airMaxVelocity) CH_velocity.x = -airMaxVelocity;
        }
        if (followRange - std::abs(position.x - player.position.x) < 10 && !followTimer.active) {
            state = CASTING;
            PlaySound(cast);
        }
    }
    if (state == CASTING) {
        if (!attackTimer.active) {
            int r = rand() % 2;
            Vector2 dir = { 1, 1 };
            if (position.x > player.position.x) dir.x = -1;
            else dir.x = 1;

            dir.y = 1;
            if (r == 1) {
                castOrb(dir);
            } else {
                castRay(dir);
            }
            attackTimer.start();
        }
        
        attackTimer.update(dt);

        if (!attackTimer.active) {
            state = FOLLOW;
            followTimer.start();
        }

    }
    printf("x %.2f t %.2f\n", position.x, position.y);
    position.x += CH_velocity.x * dt;
    position.y = player.position.y - 120.f;

    orbattack.position.x += orbattack.velocity.x * dt;
    orbattack.position.y += orbattack.velocity.y * dt;
    beamAttack.position.x += beamAttack.velocity.x * dt;
    beamAttack.position.y += beamAttack.velocity.y * dt;

    hurtbox.parentPosition.x = position.x;
    hurtbox.parentPosition.y = position.y;

    invincibilityTimer.update(dt);

    if (player.atks.active && !invincibilityTimer.active) if (hurtbox.collides(player.atks.current->hitbox)) {
        PlaySound(explosion);
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

    if (!player.invincibilityTimer.active) {
        if (orbattack.position.x < player.right() && orbattack.position.x > player.left() && orbattack.position.y < player.bottom() && orbattack.position.x > player.top()) {
            float dirX = 1;
            float dirY = -1;

            if (right() < player.left()) dirX = -1;
            if (top() > player.bottom()) dirY = 1;

            player.KB_velocity.x = -1000.f;
            if (player.atks.flipX) player.KB_velocity.x = 1000.f;

            player.takeDamage(1, 700.f, {-dirX, dirY});
            player.invincibilityTimer.start();
        }
        if (beamAttack.position.x < player.right() && beamAttack.position.x > player.left() && beamAttack.position.y < player.bottom() && beamAttack.position.x > player.top()) {
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
}
void Wizard::draw(bool debugging, float dt) {
    if (!active) return;
    if (exploded) return;
    if (debugging) {
        hurtbox.draw();
    }

    Color tint = WHITE;
    if (((int) (invincibilityTimer.elapsed * 10.f) % 2)) tint = DARKGRAY;

    anims.draw(position.x, position.y, tint);

    DrawTexturePro(orb, {0, 0, 15, 15}, {orbattack.position.x, orbattack.position.y, 60, 60}, {0, 0}, 0.f, WHITE);
    DrawTexturePro(beam, {0, 0, 41, 5}, {beamAttack.position.x, beamAttack.position.y, 82, 10}, {0, 0}, 0.f, WHITE);

}

void Wizard::kill() {
    anims.kill();
    UnloadSound(action);
    UnloadSound(hurt);
    UnloadSound(explosion);
    UnloadSound(cast);

    UnloadTexture(orb);
    UnloadTexture(beam);
}
