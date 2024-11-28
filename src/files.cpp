#include <array>
#include <cstring>

#include "files.hpp"

static const std::array<std::uint8_t, 8> pngSig{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

bool checkHeader(std::ifstream& input) {
    std::array<std::uint8_t, 8> fileHeader;
    input.read(reinterpret_cast<char*>(fileHeader.data()), fileHeader.size());

    return std::memcmp(pngSig.data(), fileHeader.data(), fileHeader.size()) == 0;
}

void writeFile(const std::filesystem::path& path, const ChunkMap& chunks, const std::vector<std::string>& requiredChunkNames) {
    std::ofstream output{ path, std::ios::binary };
    output.write(reinterpret_cast<const char*>(pngSig.data()), pngSig.size());

    for (const auto& chunkName : requiredChunkNames) {
        const auto& chunkData = chunks.at(chunkName);
        output.write(reinterpret_cast<const char*>(chunkData.data()), chunkData.size());
    }
}
