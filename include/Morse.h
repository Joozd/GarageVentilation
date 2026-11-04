
// This is an example file.

#pragma once

#include <Arduino.h>

/**
 * Outputs international Morse code on a digital GPIO pin.
 *
 * Supports A-Z, 0-9 and spaces.
 * Timing is based on a single unit:
 * - Dot: 1 unit ON
 * - Dash: 3 units ON
 * - Between symbols: 1 unit OFF
 * - Between letters: 3 units OFF
 * - Between words: 7 units OFF
 *
 * Uses blocking delays.
 */

 // This is an example file.
class Morse {
public:
    /**
     * @param unitMs Duration of one Morse time unit in milliseconds.
     * @param activeLow True if LOW activates the output.
     */
    explicit Morse(unsigned int unitMs, bool activeLow = true);

    /** Outputs one character, followed by a 3-unit OFF period. */
    void showLetter(char letter, uint8_t targetPin) const;

    /** Outputs a complete message with standard Morse spacing. */
    void showText(const char* text, uint8_t targetPin) const;

private:
    unsigned int unitMs;
    bool activeLow;

    void setOutput(uint8_t pin, bool on) const;
    void showSymbol(char symbol, uint8_t pin) const;
    void outputLetter(char letter, uint8_t pin) const;
};
