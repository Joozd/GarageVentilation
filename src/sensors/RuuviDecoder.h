#pragma once

#include "AdvertisementDecoder.h"

/** Decoder for RuuviTag BLE advertisements. */
class RuuviDecoder : public AdvertisementDecoder {
public:
    std::optional<Measurement> decode(
        const BleAdvertisement& advertisement
    ) const override;
};