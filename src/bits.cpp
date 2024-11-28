#include <array>

#include "bits.hpp"

// Adapted from: http://www.libpng.org/pub/png/spec/1.2/PNG-CRCAppendix.html

static std::array<uint32_t, 256> crcTable;
static bool crcTableComputed = false;

static void makeCRCTable() {
    for (int n = 0; n < 256; n++) {
        std::uint32_t c = static_cast<std::uint32_t>(n);
        for (int k = 0; k < 8; k++) c = c & 1 ? 0xedb88320L ^ (c >> 1) : c >> 1;

        crcTable[n] = c;
    }
    crcTableComputed = true;
}

static std::uint32_t updateCRC(std::uint32_t crc, std::span<std::uint8_t> buf) {
    std::uint32_t c = crc;
    if (!crcTableComputed) makeCRCTable();

    for (int n = 0; n < buf.size(); n++) c = crcTable[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    return c;
}

std::uint32_t calcCRC(std::span<std::uint8_t> buf) {
    return updateCRC(0xffffffffL, buf) ^ 0xffffffffL;
}
