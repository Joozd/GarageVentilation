#pragma once

#include "Measurement.h"

/** Calculates absolute humidity in g/m³ from °C and relative humidity (0–1). */
float calculateAbsoluteHumidity(const Measurement& measurement);