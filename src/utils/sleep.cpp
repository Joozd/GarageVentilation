#include <esp_sleep.h>
#include "sleep.h"


/** deep sleeps for [durationMs] millis */
void deepSleep(uint64_t durationMs) {
    esp_sleep_enable_timer_wakeup(durationMs * 1000ULL);
    esp_deep_sleep_start();
}

/** Enters light sleep for the specified number of milliseconds. */
void lightSleep(uint64_t durationMs) {
    esp_sleep_enable_timer_wakeup(durationMs * 1000ULL);
    esp_light_sleep_start();
}