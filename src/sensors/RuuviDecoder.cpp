
#include "RuuviDecoder.h"
#include "RuuviRawV2.h"

namespace {
    constexpr uint16_t ruuviManufacturerId = 0x0499;

    /** Converts raw Ruuvi temperature to degrees Celsius. */
    float decodeTemperature(int16_t raw) {
        return raw * 0.005f;
    }

    float decodeRelativeHumidity(uint16_t raw) {
        return raw * 0.000025;
    }
}

std::optional<Measurement> RuuviDecoder::decode(
    const BleAdvertisement& advertisement
) const {
    // check if this is indeed a Ruuvi package
    if (advertisement.manufacturerId != ruuviManufacturerId) 
        return std::nullopt;

    auto data = RuuviRawV2::fromBytes(advertisement.manufacturerData);
    if (!data) return std::nullopt;

    
    Measurement result{
        decodeTemperature(data->temperature),
        decodeRelativeHumidity(data->humidity)
    };
    
    return result;
}
