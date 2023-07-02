#include <../include/raylib.h>
#include <../include/timer.h>

Timer::Timer(float t) {
    active = false;
    loop = false;
    shouldTrigger = false;

    time = t;
    elapsed = 0.f;
}

void Timer::start() {
    active = true;
    elapsed = 0.f;
}

void Timer::pause() {
    active = false;
}

void Timer::play() {
    active = true;
}

void Timer::kill() {
    active = false;
    elapsed = 0.f;
}


void Timer::update(float dt) {
    if (active) elapsed += dt;
    if (elapsed > time) {
        if (!loop) kill();
        if (shouldTrigger) trigger();
    }
}