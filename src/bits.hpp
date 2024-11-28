#pragma once

#include <bit>
#include <cstdint>
#include <span>

std::uint32_t calcCRC(std::span<std::uint8_t> buf);

template <class T>
T fromBigEndian(T x) {
    if constexpr (std::endian::native == std::endian::big) return x;

    static_assert(std::endian::native == std::endian::little, "Unsupported/unknown endianness");
    return std::byteswap(x);
}
