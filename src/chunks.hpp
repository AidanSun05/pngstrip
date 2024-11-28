#pragma once

#include <fstream>
#include <tuple>
#include <unordered_map>
#include <vector>

using ChunkData = std::vector<std::uint8_t>;
using ChunkMap = std::unordered_map<std::string, ChunkData>;

std::tuple<ChunkMap, ChunkMap> readChunks(std::ifstream& input, const std::vector<std::string>& requiredChunkNames);

bool validateColorType(std::uint8_t colorType, ChunkMap& nonRequiredChunks);
