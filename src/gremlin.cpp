#include <../include/raylib.h>
#include <../include/gremlin.h>

Gremlin::Gremlin(float x, float y) : Actor(x, y, 30, 50) {
    anims = AnimationHandler();

    float sourceWidth = 30.f * 2;
    float sourceHeight = 50.f * 2;

    Texture idle = loadTextureUnloadImage("assets/player/idle.png");
    Animation _idle = Animation(1, 30.f, 50.f, sourceWidth, sourceHeight, false, -25.f, -5.f, -45.f);
    _idle.sheet = idle;
    anims.animations["idle"] = _idle;

    Texture run = loadTextureUnloadImage("assets/player/run.png");
    Animation _run = Animation(10, 30.f, 50.f, sourceWidth, sourceHeight, true, -25.f, -5.f, -45.f, 20);
    _run.sheet = run;
    anims.animations["run"] = _run;

    Texture jump = loadTextureUnloadImage("assets/player/jump.png");
    Animation _jump = Animation(1, 30.f, 50.f, sourceWidth, sourceHeight, false, -25.f, -5.f, -45.f);
    _jump.sheet = jump;
    anims.animations["jump"] = _jump;

    Texture neutral = loadTextureUnloadImage("assets/player/neutral.png");
    Animation _neutral = Animation(4, 66.f, 43.f, 66.f * 2.5f, 43.f * 2.5f, false, -110.f, -5.f, -27.f, 10);
    _neutral.sheet = neutral;
    anims.animations["neutral"] = _neutral;

    anims.setAnim("idle");

    Attack neutralA("neutral", 100.f, 110.f, hurtbox.size.x, -30.f, -100.f, 0.5f, 0.02f);
    Attack pogo("neutral", 90.f, 90.f, -45.f + hurtbox.size.x / 2.f, hurtbox.size.y, 0.f, 0.5f, 0.05f);

    atks = AttackHandler();
    atks.attacks["neutral"] = neutralA;
    atks.attacks["pogo"] = pogo;
    atks.current = &neutralA;

    

    groundAcceleration = 2000.f;
    groundMaxVelocity = 600.f;

    airAcceleration = 1500.f;
    airMaxVelocity = 400.f;

    aggroRange = 200.f;
    aggroed = false;

    attackMode = false;
    attackRange = 50.f;
}

void Gremlin::gremlinJump() {
    if (grounded) {
        grounded = false;
        CH_velocity.y = jumpVelocity;
    }
}

void Gremlin::update(Map map, Player player, float dt) {
    deaccelerateKnockback(dt);
    applyGravity(dt);
    checkIfNotGrounded(dt);

    collideWithVerticalStaticStage(map, dt);
    hurtbox.parentPosition.y = position.y;

    int input = 0;

    float dist = std::abs(player.position.x - position.x);

    if (dist < aggroRange) aggroed = true;
    
    if (!attackMode) {
        if (aggroed) {
            if (player.position.x + player.hurtbox.size.x / 2.f > position.x + hurtbox.size.x / 2.f) input = -1;
            else input = 1;
        }
    } else {
        if (dist < attackRange) {
            input = 0;
            atks.play("neutral");
        }
    }
    
    //if (IsKeyDown(ui.left)) input--;
    //if (IsKeyDown(ui.right)) input++;
    handleInput(input, dt);

    collideWithHorizontalStaticStage(map, dt);

    if (input != 0 && CH_velocity.x == 0.f) attackMode = true;

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

    if (atks.active && atks.current != NULL) anims.current = &anims.animations[atks.current->anim.c_str()];
    else if (CH_velocity.y != 0.f || !grounded) anims.current = &anims.animations["jump"];
    else if (CH_velocity.x != 0.f) anims.current = &anims.animations["run"];
    else anims.current = &anims.animations["idle"];

    anims.update(dt);

}

void Gremlin::draw(bool debugging, float dt) {
    if (debugging) {
        hurtbox.draw();
        atks.current->hitbox.draw();
    }

    anims.draw(position.x, position.y, WHITE);
}

void Gremlin::kill() {
    anims.kill();
}