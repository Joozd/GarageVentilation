#include "Fan.h"
#include <driver/gpio.h>

/** Initializes the fan GPIO in the off state. */
void Fan::initialize() {
    pinMode(_FAN_PIN, OUTPUT);
    digitalWrite(_FAN_PIN, LOW);
}

/** Starts the fan and holds the GPIO HIGH during light sleep. */
void Fan::start() {
    digitalWrite(_FAN_PIN, HIGH);
    gpio_hold_en(static_cast<gpio_num_t>(_FAN_PIN));
}

/** Releases GPIO hold and stops the fan. */
void Fan::stop() {
    gpio_hold_dis(static_cast<gpio_num_t>(_FAN_PIN));
    digitalWrite(_FAN_PIN, LOW);
}