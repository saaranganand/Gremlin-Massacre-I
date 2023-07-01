#include <../include/raylib.h>
#include <../include/attack.h>

Attack::Attack(std::string ani, float w, float h, float xO, float yO, float fXO, float dur, float timeO) {
    hitbox = Collider(ZERO, xO, yO, w, h);
    hitbox.active = false;
    anim = ani;

    xOffset = xO;
    yOffset = yO;
    flipXOffset = fXO;

    duration = dur;
    timeout = timeO;
}

AttackHandler::AttackHandler() {
    current = NULL;

    attackTime = 0.f;
    attackLength = 0.f;

    timeoutTime = 0.f;
    timeoutLength = 0.f;

    active = false;
    flipX = false;
}

void AttackHandler::play(const char* name) {
    if (active) return;

    current = &attacks[name];
    current->hitbox.active = true;
    active = true;

    attackTime = 0.f;
    attackLength = current->duration;

    timeoutTime = 0.f;
    timeoutLength = current->timeout;
}

void AttackHandler::update(float x, float y, float dt) {
    if (current == NULL) return;

    current->hitbox.parentPosition = { x, y };
    if (!active) return;

    if (flipX) {
        current->hitbox.offset.x = current->flipXOffset;
    } else {
        current->hitbox.offset.x = current->xOffset;
    }

    attackTime += dt;
    if (attackTime > attackLength) {
        attackTime = attackLength;
        current->hitbox.active = false;
    }

    if (attackTime == attackLength) {
        timeoutTime += dt;
        if (timeoutTime > timeoutLength) {
            active = false;
            current->hitbox.active = false;
        }
    }

}