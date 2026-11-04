#pragma once

#include <cstdint>
#include <optional>
#include <vector>

struct RuuviRawV2 {
    int16_t temperature;
    uint16_t humidity;
    uint16_t pressure;
    int16_t accelerationX;
    int16_t accelerationY;
    int16_t accelerationZ;
    uint16_t powerInfo;
    uint8_t movementCounter;
    uint16_t measurementSequence;
    std::vector<uint8_t> macAddress;

    static std::optional<RuuviRawV2> fromBytes(
        const std::vector<uint8_t>& data
    );
};