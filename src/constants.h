#pragma once

#include <format>
#include <Arduino.h>
#include "control/VentilationController.h"

namespace Constants {
    inline constexpr uint8_t LED = BUILTIN_LED;

    inline constexpr uint8_t FAN_OUTPUT_PIN = D8;

    inline constexpr uint64_t DEEP_SLEEP_DURATION_MS = 60 * 60  * 1000ULL;  // 60 minutes. See how this works in practice. Shorter intervals = more battery usage for ESP32, longer = miss some minutes of possible ventilation.
    inline constexpr uint64_t LIGHT_SLEEP_DURATION_MS = 10 * 60 * 1000ULL;  // 10 minutes. See how this works in practice. Shorter interval = faster fan switching off (better battery life, stop faster when a rainshower hits), longer = lower battery usage for ESP.

    inline constexpr uint32_t SCAN_TIMEOUT_MS = 15000; // How long to keep scanning for BLE devices before giving up.



    /** Minimum indoor relative humidity (0.0–1.0) required to start ventilation. */
    inline constexpr float MIN_INSIDE_HUMIDITY_FOR_VENTILATION_START = 0.66f;
    /** Minimum difference in absolute humidity (g/m³) between inside and outside. */
    inline constexpr float MIN_DELTA_FOR_VENTILATION_START = 2.0f;

        /** Minimum indoor relative humidity (0.0–1.0) required to start ventilation. */
    inline constexpr float MIN_INSIDE_HUMIDITY_FOR_VENTILATION_CONTINUE = 0.64f;
    /** Minimum difference in absolute humidity (g/m³) between inside and outside. */
    inline constexpr float MIN_DELTA_FOR_VENTILATION_CONTINUE = 1.5f;

    inline constexpr VentilationControlParameters ventStartParams { MIN_INSIDE_HUMIDITY_FOR_VENTILATION_START, MIN_DELTA_FOR_VENTILATION_START };
    inline constexpr VentilationControlParameters ventContinueParams { MIN_INSIDE_HUMIDITY_FOR_VENTILATION_CONTINUE, MIN_DELTA_FOR_VENTILATION_CONTINUE };
}