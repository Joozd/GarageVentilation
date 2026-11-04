#pragma once

#include <format>

/** deep sleeps for [durationMs] millis */
void deepSleep(uint64_t durationMs);

/** Enters light sleep for the specified number of milliseconds. */
void lightSleep(uint64_t durationMs);