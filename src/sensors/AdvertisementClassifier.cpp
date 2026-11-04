
#include "AdvertisementClassifier.h"

namespace{
    const int RUUVI_ID = 0x0499;
    const int SWITCHBOT_ID = 0x0969;
}

AdvertisementType AdvertisementClassifier::classify(
    const BleAdvertisement& advertisement
) const {
    switch(advertisement.manufacturerId){
        case RUUVI_ID:
            return AdvertisementType::Ruuvi;
        case SWITCHBOT_ID:
            return AdvertisementType::SwitchBot;
    }
    return AdvertisementType::Unknown;
}