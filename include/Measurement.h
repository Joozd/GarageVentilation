
#pragma once

/** A temperature and humidity measurement. */
struct Measurement {
    float temperatureC;
    float relativeHumidity; // as a fraction, so 80% is 0.8
};

/**
 * An inside, and an outside measurement
 */
struct CombinedMeasurement{
    Measurement measurementInside;
    Measurement measurementOutside;
};