
#include <Arduino.h>
#include <format>

#include "constants.h"

#include "sensors/BleScanner.h"
#include "sensors/MeasurementsScanner.h"

#include "hardware/fan.h"

#include "utils/sleep.h"
#include "control/VentilationController.h"


/** Prints a byte sequence as hexadecimal values. */
void printHex(const std::vector<uint8_t>& data) {
    for (uint8_t byte : data) {
        Serial.printf("%02X", byte);
    }
}

/**
 * Signal result of a scan
 * 2 blinks: No ventilation needed
 * 3 blinks: ventilation needed
 * 5 blinks: Scanning problem
 */
void blink(int blinks){
    delay(1000);
    for (int i = 0; i < blinks; i++){
        digitalWrite(Constants::LED, LOW);
        delay(300);
        digitalWrite(Constants::LED, HIGH);
        delay(300);
    }
}


namespace{
    bool thisIsRetry = false;

    MeasurementsScanner scanner;
    Fan fan(Constants::FAN_OUTPUT_PIN);
}

/**
 * ESP always wakes in a "fan off" state. "Fan on" state is in [loop]
 */
void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN,OUTPUT);    
    digitalWrite(LED_BUILTIN, HIGH);    

    /**
     * Initialize the fan, set it to OFF.
     */
    fan.initialize();

    blink(1);
    

    auto measurement = scanner.scan(20000);  
    if(!measurement) {// TODO handle timeout, perhaps via callback
        blink(5);
        deepSleep(Constants::DEEP_SLEEP_DURATION_MS);; // for now: no data = no ventilation.                  
    }

    if (!shouldVentilate(*measurement, Constants::ventStartParams)){
        // Go to deep sleep if no ventilation needed. This will reboot after [DEEP_SLEEP_DURATION_MS], returning to this [setup].
        blink(2);
        deepSleep(Constants::DEEP_SLEEP_DURATION_MS);
        
    }
    else{ // here for clarity, the [if] has an early exit through deepSleep()        
        fan.start();
    }    
}

/** Scans and classifies nearby BLE advertisements. */



/** Receives and processes advertisements for one scan cycle. */
void loop() {
    blink(3);
    lightSleep(Constants::LIGHT_SLEEP_DURATION_MS);

    //TODO write vantilation logic. Check every 20 minutes, and normal sleep?
    auto measurement = scanner.scan(15000);
    if(!measurement){
        //retry once, two failures = fan off
        if(thisIsRetry){
            blink(5);
            fan.stop();
            deepSleep(Constants::DEEP_SLEEP_DURATION_MS); // reboot after deel sleep.
        }
        else{
            thisIsRetry=true;
        }            
    }
    else{
        thisIsRetry = false;
        if(!shouldVentilate(*measurement, Constants::ventContinueParams)){
            blink(2);
            fan.stop();
            deepSleep(Constants::DEEP_SLEEP_DURATION_MS); // reboot after deel sleep.
        }
    }
    // no early exits, keep ventilating for another LIGHT_SLEEP_DURATION_MS millis.    
}