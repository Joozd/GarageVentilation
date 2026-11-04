
#include "ByteReader.h"

ByteReader::ByteReader(
    const std::vector<uint8_t>& data,
    size_t startOffset
) : data(data),
    offset(startOffset <= data.size() ? startOffset : data.size()) {}

bool ByteReader::hasBytes(size_t count) const {
    return count <= remaining();
}

size_t ByteReader::remaining() const {
    return data.size() - offset;
}

std::optional<uint8_t> ByteReader::readUInt8() {
    if (!hasBytes(1)) return std::nullopt;

    return data[offset++];
}

std::optional<uint16_t> ByteReader::readUInt16BE() {
    if (!hasBytes(2)) return std::nullopt;

    uint16_t value =
        (static_cast<uint16_t>(data[offset]) << 8) |
        data[offset + 1];

    offset += 2;
    return value;
}

std::optional<uint16_t> ByteReader::readUInt16LE() {
    if (!hasBytes(2)) return std::nullopt;

    uint16_t value =
        static_cast<uint16_t>(data[offset]) |
        (static_cast<uint16_t>(data[offset + 1]) << 8);

    offset += 2;
    return value;
}

int16_t ByteReader::toSigned16(uint16_t value) {
    int32_t signedValue = value >= 0x8000
        ? static_cast<int32_t>(value) - 0x10000
        : static_cast<int32_t>(value);

    return static_cast<int16_t>(signedValue);
}

std::optional<int16_t> ByteReader::readInt16BE() {
    auto value = readUInt16BE();
    if (!value) return std::nullopt;

    return toSigned16(*value);
}

std::optional<int16_t> ByteReader::readInt16LE() {
    auto value = readUInt16LE();
    if (!value) return std::nullopt;

    return toSigned16(*value);
}

std::optional<std::vector<uint8_t>> ByteReader::readBytes(size_t count) {
    if (!hasBytes(count)) return std::nullopt;

    auto begin = data.begin() + offset;
    std::vector<uint8_t> result(begin, begin + count);

    offset += count;
    return result;
}
