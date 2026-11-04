#pragma once

#include "AdvertisementDecoder.h"

/** Decoder for Switchbot BLE advertisements. */
class SwitchbotDecoder : public AdvertisementDecoder {
public:
    std::optional<Measurement> decode(
        const BleAdvertisement& advertisement
    ) const override;
};