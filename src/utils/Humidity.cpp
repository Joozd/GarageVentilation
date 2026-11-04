
#include <cmath>
#include "Humidity.h"
#include "Measurement.h"

/**
 * Calculates absolute humidity using the Sonntag (1990) Magnus formula.
 *
 * @param measurement Temperature in degrees Celsius and relative humidity
 *                    as a fraction between 0.0 and 1.0.
 * @return Absolute humidity in grams per cubic meter (g/m³).
 */
float calculateAbsoluteHumidity(const Measurement& measurement){
    float satVapPres = 6.112f * std::exp(   17.62f*measurement.temperatureC 
                                                       / 
                                          (measurement.temperatureC + 243.12f));

    // in g/m3
    float absHumid = (216.7f * measurement.relativeHumidity * satVapPres)
                                    /
                         (273.15f + measurement.temperatureC);

    return absHumid;    
}