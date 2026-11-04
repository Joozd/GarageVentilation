#pragma once

#include <optional>
#include "BleAdvertisement.h"
#include "Measurement.h"

/** Interface for decoding BLE sensor advertisements. */
class AdvertisementDecoder {
public:
    virtual ~AdvertisementDecoder() = default;

    /** Decodes an advertisement, or returns nullopt on failure. */
    virtual std::optional<Measurement> decode(
        const BleAdvertisement& advertisement
    ) const = 0;
};