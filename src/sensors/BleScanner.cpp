
#include "BleScanner.h"

#include <NimBLEDevice.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {
    constexpr size_t QUEUE_CAPACITY = 16;
    constexpr size_t MAX_DATA_SIZE = 128;

    /**
     * Fixed-size advertisement representation.
     *
     * Contains no pointers or dynamically allocated members,
     * so FreeRTOS can safely copy it into a queue.
     */
    struct RawAdvertisement {
        char address[18] = {};
        int rssi = 0;
        uint16_t manufacturerId = 0;
        uint8_t dataSize = 0;
        uint8_t data[MAX_DATA_SIZE] = {};
    };
}

/** Contains all NimBLE and FreeRTOS implementation details. */
struct BleScanner::Impl {
    QueueHandle_t queue = nullptr;
    bool btInitialized = false;

    /** Receives advertisements from the NimBLE host task. */
    class Callbacks : public NimBLEScanCallbacks {
    public:
        explicit Callbacks(Impl& owner) : owner(owner) {}

        void onResult(
            const NimBLEAdvertisedDevice* device
        ) override {
            if (!device->haveManufacturerData()) return;

            std::string manufacturerData =
                device->getManufacturerData();

            // First two bytes contain the manufacturer ID.
            if (manufacturerData.size() < 2) return;

            size_t payloadSize = manufacturerData.size() - 2;

            // Ignore packets that do not fit our buffer.
            if (payloadSize > MAX_DATA_SIZE) return;

            RawAdvertisement advertisement;

            std::string address =
                device->getAddress().toString();

            std::snprintf(
                advertisement.address,
                sizeof(advertisement.address),
                "%s",
                address.c_str()
            );

            advertisement.rssi = device->getRSSI();

            advertisement.manufacturerId =
                static_cast<uint8_t>(manufacturerData[0]) |
                (static_cast<uint16_t>(
                    static_cast<uint8_t>(manufacturerData[1])
                ) << 8);

            advertisement.dataSize =
                static_cast<uint8_t>(payloadSize);

            if (payloadSize > 0) {
                std::memcpy(
                    advertisement.data,
                    manufacturerData.data() + 2,
                    payloadSize
                );
            }

            // Do not block the Bluetooth task if the queue is full.
            xQueueSend(owner.queue, &advertisement, 0);
        }

    private:
        Impl& owner;
    };

    Callbacks callbacks;

    Impl() : callbacks(*this) {}

    /** Removes unread advertisements from the queue. */
    void clearQueue() {
        if (queue != nullptr) {
            xQueueReset(queue);
        }
    }
};

BleScanner::BleScanner()
    : impl(std::make_unique<Impl>()) {}

BleScanner::~BleScanner() {
    // Normally called after scanning has already stopped.
    if (!stop()) {
        // A failed BLE shutdown is not safely recoverable here.
        // Never destroy a queue that a live callback might use.
        std::abort();
    }

    if (impl->queue != nullptr) {
        vQueueDelete(impl->queue);
    }
}

/** Initializes NimBLE and configures passive scanning. */
bool BleScanner::initializeBTStack() {
    if (impl->btInitialized) return true;

    if (!NimBLEDevice::init("")) {
        // Attempt to clean up a partially initialized stack.
        NimBLEDevice::deinit(true);
        return false;
    }

    impl->btInitialized = true;

    NimBLEScan* scan = NimBLEDevice::getScan();

    scan->setScanCallbacks(&impl->callbacks, false);
    scan->setActiveScan(false);
    scan->setInterval(100);
    scan->setWindow(99);
    scan->setMaxResults(0);

    return true;
}

/** Releases all NimBLE resources. */
bool BleScanner::deinitializeBTStack() {
    if (!impl->btInitialized) return true;

    if (!NimBLEDevice::deinit(true)) {
        return false;
    }

    impl->btInitialized = false;
    return true;
}

/** Starts scanning with a freshly initialized Bluetooth stack. */
bool BleScanner::start() {
    // Do not start another scan while Bluetooth is active.
    if (impl->btInitialized) return false;

    // Create the queue only once.
    if (impl->queue == nullptr) {
        impl->queue = xQueueCreate(
            QUEUE_CAPACITY,
            sizeof(RawAdvertisement)
        );

        if (impl->queue == nullptr) return false;
    }

    impl->clearQueue();

    if (!initializeBTStack()) return false;

    NimBLEScan* scan = NimBLEDevice::getScan();

    // Duration 0 means continuous scanning until stop().
    if (!scan->start(0, false, true)) {
        stop(); // Clean up even if the scan could not start.
        return false;
    }

    return true;
}

/** Waits for the next queued advertisement. */
std::optional<BleAdvertisement>
BleScanner::nextAdvertisement(uint32_t timeoutMs) {
    if (!impl->btInitialized) return std::nullopt;

    RawAdvertisement raw;

    if (xQueueReceive(
            impl->queue,
            &raw,
            pdMS_TO_TICKS(timeoutMs)
        ) != pdTRUE) {
        return std::nullopt;
    }

    return BleAdvertisement{
        std::string(raw.address),
        raw.rssi,
        raw.manufacturerId,
        std::vector<uint8_t>(
            raw.data,
            raw.data + raw.dataSize
        )
    };
}

/** Stops scanning and completely deinitializes Bluetooth. */
bool BleScanner::stop() {
    if (!impl->btInitialized) {
        impl->clearQueue();
        return true;
    }

    NimBLEDevice::getScan()->stop();

    if (!deinitializeBTStack()) {
        return false;
    }

    // The callback can no longer access the queue.
    impl->clearQueue();
    return true;
}
