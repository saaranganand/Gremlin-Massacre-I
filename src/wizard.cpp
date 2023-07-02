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
    beamAttack = { { position.x, position.y }, { dir.x * 1200, dir.y * 1200}};
}

void Wizard::reset(float x, float y) {
    position.x = x;
    position.y = y;

    state = IDLE;

    health = maxHealth;
}

Wizard::Wizard(float x = 0.f, float y = 0.f) {
    anims = AnimationHandler();

    Vector2 dest = {200,200};
    Vector2 offset = {0,-40};
    float fxOff = -65.f;

    createAnimation("gandalf/", "idle", 1, {48,70}, dest, true, offset, fxOff);
    createAnimation("gandalf/", "cast", 4, {68,70}, dest, true, offset, fxOff);

    anims.setAnim("idle");

    state = IDLE;

    orbattack = { { -10, -10 }, { 0, 0}};
    beamAttack = { { -10, -10 }, { 0, 0}};

    orb = loadTextureUnloadImage("gandalf/orb.png");
    beam = loadTextureUnloadImage("gandalf/beam.png");

    groundAcceleration = 2000.f;
    groundMaxVelocity = 600.f;

    airAcceleration = 2000.f;
    airMaxVelocity = 700.f;

    followRange = 200.f;

    invincibilityTimer = Timer(0.5f);
    attackTimer = Timer(1.5f);

    maxHealth = 30;
    health = 30;

    action = LoadSound("assets/wizard/beam.mp3");
    hurt = LoadSound("assets/wizard/hurt.wav");
    explosion = LoadSound("assets/wizard/explosion.wav");
    cast = LoadSound("assets/wizard/laugh.mp3");
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
    if (state == FOLLOW) {
        if (position.x < player.position.x) {
            CH_velocity.x += airAcceleration * dt;
            if (CH_velocity.x > airMaxVelocity) CH_velocity.x = airMaxVelocity;
        } else if (position.x >= player.position.x) {
            CH_velocity.x -= airAcceleration * dt;
            if (CH_velocity.x < -airMaxVelocity) CH_velocity.x = -airMaxVelocity;
        }
        if (followRange - std::abs(position.x - player.position.x) < 10 && std::abs(position.x - player.position.x > followRange * 0.9) ) {
            state = CASTING;
            PlaySound(cast);
        }
    }
    if (state == CASTING) {
        if (!attackTimer.active) {
            int r = rand() % 2;
            Vector2 dir = {};
            if (r == 1) {
                castOrb(dir);
            } else {
                castRay(dir);
            }
            attackTimer.start();
        }
        
        attackTimer.update(dt);

        if (!attackTimer.active) state == FOLLOW;

    }
}
void Wizard::draw(bool debugging, float dt) {
    if (exploded) return;
    if (debugging) {
        hurtbox.draw();
        atks.current->hitbox.draw();
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
