
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

/**
 * Sequential reader for binary data.
 *
 * Supports explicit big-endian and little-endian integer reads.
 * Failed reads return std::nullopt without advancing the cursor.
 *
 * The source vector must outlive this reader.
 */
class ByteReader {
public:
    /**
     * Creates a reader starting at the specified offset.
     *
     * Offsets beyond the buffer are clamped to its end.
     */
    explicit ByteReader(
        const std::vector<uint8_t>& data,
        size_t startOffset = 0
    );

    /** Reads one unsigned byte. */
    std::optional<uint8_t> readUInt8();

    /** Reads an unsigned 16-bit big-endian integer. */
    std::optional<uint16_t> readUInt16BE();

    /** Reads an unsigned 16-bit little-endian integer. */
    std::optional<uint16_t> readUInt16LE();

    /** Reads a signed 16-bit big-endian integer. */
    std::optional<int16_t> readInt16BE();

    /** Reads a signed 16-bit little-endian integer. */
    std::optional<int16_t> readInt16LE();

    /** Returns the number of unread bytes. */
    size_t remaining() const;

    /** Reads count bytes into a vector. */
    std::optional<std::vector<uint8_t>> readBytes(size_t count);

private:
    const std::vector<uint8_t>& data;
    size_t offset;

    bool hasBytes(size_t count) const;

    /** Converts a two's-complement bit pattern to int16_t. */
    static int16_t toSigned16(uint16_t value);
};

