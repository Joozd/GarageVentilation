
#pragma once

#include <Arduino.h>

/**
 * Controls a fan through a GPIO-driven MOSFET.
 *
 * The GPIO pin is configured during begin(), which must be called
 * once from setup() before using start() or stop().
 *
 * When started, GPIO hold keeps the output HIGH during ESP32 light sleep,
 * allowing the fan to continue running while the CPU sleeps.
 *
 * The fan is switched off by releasing GPIO hold and setting the pin LOW.
 *
 * @note Do not initialize GPIO hardware from the constructor.
 *       Global objects are constructed before Arduino setup().
 * @note GPIO hold is intended for light sleep. The fan must be
 *       explicitly switched off before entering deep sleep.
 */
class Fan {
public:
    /**
     * Constructs a fan controller for the specified GPIO pin.
     *
     * Does not initialize the GPIO or change its output state.
     *
     * @param fanPin GPIO pin connected to the MOSFET gate.
     */
    explicit Fan(uint8_t fanPin) : _FAN_PIN(fanPin) {}

    /**
     * Configures the GPIO as an output and initially sets it LOW.
     *
     * Must be called from setup() before start() or stop().
     */
    void initialize();

    /**
     * Starts the fan by setting the GPIO HIGH.
     *
     * Enables GPIO hold to maintain the output during light sleep.
     * Requires initialize() to have been called.
     */
    void start();

    /**
     * Stops the fan by releasing GPIO hold and setting the GPIO LOW.
     *
     * Requires initialize() to have been called.
     */
    void stop();

private:
    /** Fixed GPIO pin controlling the fan MOSFET. */
    const uint8_t _FAN_PIN;
};