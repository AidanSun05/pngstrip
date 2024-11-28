#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "chunks.hpp"

bool checkHeader(std::ifstream& input);

void writeFile(const std::filesystem::path& path, const ChunkMap& chunks, const std::vector<std::string>& requiredChunkNames);
