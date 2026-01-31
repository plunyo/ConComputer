#include "timer/timer.h"

Timer CreateTimer(float interval, bool repeat, TimerCallback callback, void* callbackContext) {
    Timer t;

    t.elapsed = 0.0f;
    t.interval = interval;
    t.repeat = repeat;
    t.active = true;
    t.callback = callback;
    t.callbackContext = callbackContext;

    return t;
}

void UpdateTimer(Timer* timer, float deltaTime) {
    if (!timer->active || !timer->callback) return;

    timer->elapsed += deltaTime;
    if (timer->elapsed >= timer->interval) {
        timer->callback(timer->callbackContext);
        if (timer->repeat) {
            timer->elapsed -= timer->interval;
        } else {
            timer->active = false;
        }
    }
}

void ResetTimer(Timer* timer) {
    timer->elapsed = 0.0f;
    timer->active = true;
}

void PlayTimer(Timer* timer) {
    timer->active = true;
}

void StopTimer(Timer* timer) {
    timer->active = false;
}
