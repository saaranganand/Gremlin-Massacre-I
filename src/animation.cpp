#include <../include/raylib.h>
#include <../include/animation.h>
#include <../include/defs.h>

Animation::Animation(int fms, float sW, float sH, float dW, float dH, bool lop, float fXO, float xO, float yO, int f) {
    sheet = {};
    source = { 0.f, 0.f, sW, sH };

    sourceWidth = sW;
    sourceHeight = sH;

    destWidth = dW;
    destHeight = dH;

    flipXOffset = fXO;
    xOffset = xO;
    yOffset = yO;

    loop = lop;
    frames = fms;

    fps = f;
}

Rectangle Animation::getDestFrame(float x, float y) {
    return (Rectangle) { x, y, destWidth, destHeight };
}

void Animation::kill() {
    UnloadTexture(sheet);
}

AnimationHandler::AnimationHandler() {
    current = NULL;

    done = false;
    flipX = false;
    frame = 0;
    time = 0.f;
}

void AnimationHandler::setAnim(const char* name) {
    current = &animations[name];
    done = false;
}

void AnimationHandler::update(float dt) {
    time += dt;
    if (time * current->fps > 1) {
        frame++;
        time = 0.f;
    }
    if (frame == current->frames && current->loop) frame = 0;
    else done = true;
    clamp(frame, 0, current->frames - 1);
}

void AnimationHandler::draw(float x, float y, Color tint) {
    Rectangle dest = current->getDestFrame(x, y);
    dest.y += current->yOffset;
    if (!flipX) dest.x += current->xOffset;
    else dest.x += current->flipXOffset;

    Rectangle src = current->source;
    src.x += current->sourceWidth * frame;
    if (flipX) src.width *= -0.99;

    DrawTexturePro(current->sheet, src, dest, ZERO, 0.f, tint);
}

void AnimationHandler::kill() {
    std::map<const char*, Animation>::iterator iter;

    //for (iter = animations.begin(); iter != animations.end(); iter++) {
        //iter->second.kill();
    //}
}