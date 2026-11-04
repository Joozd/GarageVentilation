
// This is an example file.

#include "Morse.h"
#include <ctype.h>

namespace {
    // A-Z followed by 0-9.
    constexpr const char* MORSE_CODES[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
        "....", "..", ".---", "-.-", ".-..", "--", "-.",
        "---", ".--.", "--.-", ".-.", "...", "-", "..-",
        "...-", ".--", "-..-", "-.--", "--..",
        "-----", ".----", "..---", "...--", "....-",
        ".....", "-....", "--...", "---..", "----."
    };

    /** Returns the Morse pattern for a supported character. */
    const char* getMorseCode(char letter) {
        char c = toupper(static_cast<unsigned char>(letter));

        if (c >= 'A' && c <= 'Z') {
            return MORSE_CODES[c - 'A'];
        }
        if (c >= '0' && c <= '9') {
            return MORSE_CODES[26 + c - '0'];
        }
        return nullptr;
    }
}

Morse::Morse(unsigned int unitMs, bool activeLow)
    : unitMs(unitMs), activeLow(activeLow) {}

void Morse::setOutput(uint8_t pin, bool on) const {
    digitalWrite(pin, (on != activeLow) ? HIGH : LOW);
}

void Morse::showSymbol(char symbol, uint8_t pin) const {
    setOutput(pin, true);
    delay(unitMs * (symbol == '.' ? 1UL : 3UL));
    setOutput(pin, false);
}

void Morse::outputLetter(char letter, uint8_t pin) const {
    const char* code = getMorseCode(letter);
    if (code == nullptr) return;

    for (size_t i = 0; code[i] != '\0'; ++i) {
        if (i > 0) {
            delay(unitMs);
        }
        showSymbol(code[i], pin);
    }
}

void Morse::showLetter(char letter, uint8_t targetPin) const {
    outputLetter(letter, targetPin);
    setOutput(targetPin, false);
    delay(unitMs * 3UL);
}

void Morse::showText(const char* text, uint8_t targetPin) const {
    bool previousWasLetter = false;
    bool wordGap = false;

    setOutput(targetPin, false);

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == ' ') {
            if (previousWasLetter) {
                wordGap = true;
            }
            continue;
        }

        if (getMorseCode(text[i]) == nullptr) {
            continue;
        }

        if (previousWasLetter) {
            delay(unitMs * (wordGap ? 7UL : 3UL));
        }

        outputLetter(text[i], targetPin);
        previousWasLetter = true;
        wordGap = false;
    }

    // End the message with a letter-length OFF period.
    setOutput(targetPin, false);
    delay(unitMs * 3UL);
}