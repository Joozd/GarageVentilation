#pragma once

/**
 * @brief Configuration parameters for the ventilation controller.
 */
struct VentilationControlParameters {
    /** Minimum indoor relative humidity (0.0–1.0) required for ventilation. */
    float minimumHumidityInside;

    /** Minimum indoor-outdoor absolute humidity difference in g/m³ required for ventilation. */
    float minimumAbsoluteHumidityDelta;
};