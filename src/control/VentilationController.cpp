#include "Measurement.h"
#include "utils/Humidity.h"
#include "VentilationController.h"
#include "VentilationControlParameters.h"

/**
 * Determines whether ventilation is beneficial based on humidity.
 *
 * Ventilation is recommended when indoor relative humidity meets the
 * configured minimum and indoor absolute humidity exceeds outdoor
 * absolute humidity by more than the configured threshold.
 *
 * @param measurements Indoor and outdoor measurements.
 * @param params Humidity thresholds used for the decision.
 * @return True if both conditions are satisfied, false otherwise.
 */
bool shouldVentilate(const CombinedMeasurement& measurements, const VentilationControlParameters& params){
    if(measurements.measurementInside.relativeHumidity < params.minimumHumidityInside)
        return false;

    float absOutside = calculateAbsoluteHumidity(measurements.measurementOutside);
    float absInside = calculateAbsoluteHumidity(measurements.measurementInside);

    return absInside - absOutside > params.minimumAbsoluteHumidityDelta;
}