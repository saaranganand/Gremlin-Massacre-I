#ifndef TIMER_H_
#define TIMER_H_

#include <functional>

struct Timer {
    bool active;
    bool loop;
    bool shouldTrigger;

    float time;
    float elapsed;

    std::function<void()> trigger;

    Timer(float t = 0.f);

    void start();
    void pause();
    void play();
    void kill();

    void update(float dt);

};

#endif