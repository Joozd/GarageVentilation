
#include <cstdint>
#include <string>
#include <vector>
#include "SwitchbotDecoder.h"

namespace {
    constexpr uint16_t switchbotManufacturerId = 0x0969;

    /** Converts raw Ruuvi temperature to degrees Celsius. */
    float getTempFromBytes(const std::vector<uint8_t>& bytes) {
        auto len = bytes.size();
        auto decimalPart = bytes[len-3] & 0x0F;
        uint8_t intpartRaw = bytes[len - 2];
        int intpart = (intpartRaw & 0x7F) * ((intpartRaw & 0x80) ? 1 : -1);
        float result = intpart + decimalPart * 0.1f * ((intpartRaw & 0x80) ? 1 : -1);       

        return result;
    }

    float decodeRelativeHumidity(const std::vector<uint8_t>& bytes) {
        uint8_t raw = bytes.back();
        uint8_t humidIntPercentage = raw & 0x7F;
        auto humid = humidIntPercentage * 0.01f;

        return humid;
    }
}

std::optional<Measurement> SwitchbotDecoder::decode(
    const BleAdvertisement& advertisement
) const {
    // check if this is indeed a Ruuvi package
    if (advertisement.manufacturerId != switchbotManufacturerId) 
        return std::nullopt;

    // standard data package is more bytes, but only th elast 3 have info we want
    if(advertisement.manufacturerData.size() < 3)
        return std::nullopt;

    
    Measurement result{
        getTempFromBytes(advertisement.manufacturerData),
        decodeRelativeHumidity(advertisement.manufacturerData)
    };
    
    return result;
}