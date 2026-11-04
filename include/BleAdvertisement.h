
#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** BLE advertisement data, independent of the BLE library. */
struct BleAdvertisement {
    std::string address;
    int rssi;
    uint16_t manufacturerId;
    std::vector<uint8_t> manufacturerData;
};
