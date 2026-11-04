#include <Arduino.h>

#include "MeasurementsScanner.h"
#include "Measurement.h"

std::optional<CombinedMeasurement> MeasurementsScanner::scan(uint32_t timeoutMs){
    if (!scanner.start()) return std::nullopt;

    uint32_t startedAt = millis();
    std::optional<Measurement> measurementInside;
    std::optional<Measurement> measurementOutside;

    digitalWrite(LED_BUILTIN, LOW);
    while (!measurementInside || !measurementOutside) {
        
        uint32_t elapsed = millis() - startedAt;
        if (elapsed >= timeoutMs) break;

        uint32_t remaining = timeoutMs - elapsed;
        auto advertisement = scanner.nextAdvertisement(remaining);
        if (!advertisement) break;

        switch (classifier.classify(*advertisement)) {
            case AdvertisementType::Ruuvi: {
                measurementOutside = ruuviDecoder.decode(*advertisement);
                break;
            }

            case AdvertisementType::SwitchBot: {
                measurementInside = switchbotDecoder.decode(*advertisement);
                break;
            }

            case AdvertisementType::Unknown: {
                // explicit noop
            }
        }        
    }
    //LED off
    digitalWrite(LED_BUILTIN, HIGH);

    if (!scanner.stop()) {
        // Bluetooth could not be shut down safely. Reboot the entire board to try and fix that.
        pinMode(LED_BUILTIN,OUTPUT);    
        for(int x=1; x<10; x++){
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }

        ESP.restart();
}

    if (!measurementInside || !measurementOutside){ // TODO decide what we want with missing broadcasts after timeout. For now we just say "No vantilation".
        return std::nullopt;
    }

    return CombinedMeasurement{
        *measurementInside, 
        *measurementOutside
    };
}