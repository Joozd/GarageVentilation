
#pragma once

#include "BleAdvertisement.h"

/** Supported advertisement types. */
enum class AdvertisementType {
    Ruuvi,
    SwitchBot,
    Unknown
};

/** Classifies BLE advertisements by manufacturer. */
class AdvertisementClassifier {
public:
    /** Returns the recognized advertisement type. */
    AdvertisementType classify(
        const BleAdvertisement& advertisement
    ) const;
};