#include <algorithm>
#include <format>
#include <stdexcept>

#include "chunks.hpp"

#include "bits.hpp"

static std::tuple<bool, bool> checkPLTE(const ChunkMap& nonRequiredChunks, std::uint8_t colorType) {
    switch (colorType) {
        case 2:
        case 6:
            // PLTE is optional
            return { true, false };
        case 3:
            // PLTE must be present
            return { nonRequiredChunks.contains("PLTE"), true };
        case 0:
        case 4:
            // PLTE must not be present
            return { !nonRequiredChunks.contains("PLTE"), false };
        default:
            throw std::runtime_error{ "Unknown color type in IHDR chunk" };
    }
}

std::tuple<ChunkMap, ChunkMap> readChunks(std::ifstream& input, const std::vector<std::string>& requiredChunkNames) {
    ChunkMap requiredChunks;
    ChunkMap nonRequiredChunks;

    // PNG chunk layout:
    // | Length (4 bytes) | Type (4 bytes) | Data (length bytes) | CRC (4 bytes) |
    // CRC calcuated from ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    while (input.peek() != EOF) {
        ChunkData data(4);

        // Get length first
        std::uint32_t length;
        input.read(reinterpret_cast<char*>(data.data()), 4);
        std::memcpy(&length, data.data(), 4);
        length = fromBigEndian(length);

        // Read rest of chunk
        data.resize(length + 12);
        input.read(reinterpret_cast<char*>(data.data() + 4), length + 8);

        // Extract type string
        std::string type(4, '\0');
        std::memcpy(type.data(), data.data() + 4, type.size());

        // Extract CRC
        std::uint32_t crc;
        std::memcpy(&crc, data.data() + length + 8, 4);
        crc = fromBigEndian(crc);

        // Validate CRC
        std::uint32_t calculatedCRC = calcCRC({ data.begin() + 4, length + 4 });
        if (calculatedCRC != crc) throw std::runtime_error{ std::format("CRC mismatch in {} chunk", type) };

        // Add to appropriate list
        auto& listToAdd = std::ranges::find(requiredChunkNames, type) == requiredChunkNames.end() ? nonRequiredChunks[type] : requiredChunks[type];
        listToAdd.insert(listToAdd.end(), data.begin(), data.end());
    }

    return { requiredChunks, nonRequiredChunks };
}

bool validateColorType(std::uint8_t colorType, ChunkMap& nonRequiredChunks) {
    auto [checkSuccess, shouldIncludePLTE] = checkPLTE(nonRequiredChunks, colorType);
    if (!checkSuccess)
        throw std::runtime_error{ std::format("Invalid PLTE chunk for color type {}", colorType) };

    return shouldIncludePLTE;
}
