
#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "BleAdvertisement.h"

/**
 * Receives BLE advertisements one at a time.
 *
 * Bluetooth is initialized when scanning starts and completely
 * shut down when scanning stops, allowing light sleep between scans.
 *
 * Uses an internal FreeRTOS queue to transfer advertisements from
 * the NimBLE callback to the calling task.
 *
 * Call methods from the same application task.
 * This scanner must be the only owner of the NimBLE stack.
 */
class BleScanner {
public:
    BleScanner();
    ~BleScanner();

    BleScanner(const BleScanner&) = delete;
    BleScanner& operator=(const BleScanner&) = delete;

    /**
     * Initializes Bluetooth and starts scanning.
     *
     * Creates the internal queue on first use.
     *
     * @return True if scanning started successfully.
     */
    bool start();

    /**
     * Waits for the next BLE advertisement.
     *
     * Only advertisements containing manufacturer data are returned.
     *
     * @param timeoutMs Maximum waiting time in milliseconds.
     * @return An advertisement, or std::nullopt on timeout or if not scanning.
     */
    std::optional<BleAdvertisement> nextAdvertisement(uint32_t timeoutMs);

    /**
     * Stops scanning and completely shuts down Bluetooth.
     *
     * Safe to call when Bluetooth is already stopped.
     *
     * @return True if Bluetooth is fully deinitialized.
     */
    bool stop();

private:
    /** Initializes NimBLE and registers the advertisement callback. */
    bool initializeBTStack();

    /** Shuts down NimBLE, including its scan object. */
    bool deinitializeBTStack();

    struct Impl;
    std::unique_ptr<Impl> impl;
};
