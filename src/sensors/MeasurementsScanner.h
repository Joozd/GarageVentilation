
#include "Measurement.h"
#include "sensors/BleScanner.h"
#include "sensors/AdvertisementClassifier.h"
#include "sensors/RuuviDecoder.h"
#include "sensors/SwitchbotDecoder.h"

/**
 * Coordinates BLE scanning and decoding to collect measurements
 * from the indoor SwitchBot and outdoor Ruuvi sensors.
 */
class MeasurementsScanner {
public:
    /**
     * Scans for valid measurements from both sensors.
     *
     * Stops scanning when both measurements have been received
     * or the specified timeout has elapsed.
     *
     * @param timeoutMs Maximum scan duration in milliseconds.
     * @return Both measurements, or std::nullopt if either is missing.
     */
    std::optional<CombinedMeasurement> scan(uint32_t timeoutMs);

private:
    BleScanner scanner;
    AdvertisementClassifier classifier;
    RuuviDecoder ruuviDecoder;
    SwitchbotDecoder switchbotDecoder;
};