#include <optional>
#include <vector>
#include "RuuviRawV2.h"
#include "utils/ByteReader.h"


// https://docs.ruuvi.com/communication/bluetooth-advertisements/data-format-5-rawv2
/**
 * Create a `RuuviRawV2` object from bytes.
 * returns nullopt if invalid data was received (invalid for any reason)
 */
std::optional<RuuviRawV2> RuuviRawV2::fromBytes(
    const std::vector<uint8_t>& data
) {
    // Check correct data length: 24 bytes and byte 0 is 0x05 (which is the identification of RAWv2). RAWv2 
    if (data.size() != 24 || data[0] != 0x05) {
        return std::nullopt;
    }

    ByteReader reader(data, 1);

    auto temperature = reader.readInt16BE(); if (!temperature) return std::nullopt;    
    auto humidity = reader.readUInt16BE(); if (!humidity) return std::nullopt;
    auto pressure = reader.readUInt16BE(); if (!pressure) return std::nullopt;

    auto accelerationX = reader.readInt16BE(); if (!accelerationX) return std::nullopt;
    auto accelerationY = reader.readInt16BE(); if (!accelerationY) return std::nullopt;    
    auto accelerationZ = reader.readInt16BE(); if (!accelerationZ) return std::nullopt;

    auto powerInfo = reader.readUInt16BE(); if (!powerInfo) return std::nullopt;
    auto movementCounter = reader.readUInt8(); if (!movementCounter) return std::nullopt;
    auto measurementSequence = reader.readUInt16BE(); if (!measurementSequence) return std::nullopt;
    auto macAddres = reader.readBytes(6); if (!macAddres) return std::nullopt;

    RuuviRawV2 result{
        *temperature,
        *humidity,
        *pressure,

        *accelerationX,
        *accelerationY,
        *accelerationZ,

        *powerInfo,
        *movementCounter,
        *measurementSequence,
        *macAddres
    };    

    return result;
}