#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <format>

#include "files.hpp"

// http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html
// https://en.wikipedia.org/wiki/PNG

std::size_t origSizeTotal = 0;
std::size_t stripSizeTotal = 0;

void stripFile(const std::filesystem::directory_entry& file, bool overwrite) {
    auto path = file.path();

    if (path.extension() != ".png") return;
    std::cout << path << '\n';
    origSizeTotal += std::filesystem::file_size(file);

    std::ifstream input{ path, std::ios::binary };
    if (!checkHeader(input)) throw std::runtime_error{ "Invalid PNG file" };

    std::vector<std::string> requiredChunkNames{ "IHDR", "IDAT", "IEND" };
    auto [requiredChunks, nonRequiredChunks] = readChunks(input, requiredChunkNames);

    for (const auto& i : requiredChunkNames)
        if (!requiredChunks.contains(i))
            throw std::runtime_error{ std::format("Missing required chunk {}", i) };

    // Different color types have different PLTE requirements
    std::uint8_t colorType = requiredChunks["IHDR"][17];
    std::cout << "Found color type: " << static_cast<int>(colorType) << '\n';

    // Make PLTE (palette) required if necessary
    bool shouldIncludePLTE = validateColorType(colorType, nonRequiredChunks);
    if (shouldIncludePLTE) {
        requiredChunks["PLTE"] = nonRequiredChunks["PLTE"];
        nonRequiredChunks.erase("PLTE");
        requiredChunkNames.insert(requiredChunkNames.begin() + 1, "PLTE"); // Before IDAT
    }

    // tRNS (transparency) should be copied when it exists
    if (nonRequiredChunks.contains("tRNS")) {
        requiredChunks["tRNS"] = nonRequiredChunks["tRNS"];
        nonRequiredChunks.erase("tRNS");
        requiredChunkNames.insert(requiredChunkNames.end() - 2, "tRNS");
    }

    std::cout << "Found required chunks: ";
    for (const auto& [k, _] : requiredChunks) std::cout << k << ' ';

    std::cout << "\nFound non-required chunks: ";
    for (const auto& [k, _] : nonRequiredChunks) std::cout << k << ' ';
    std::cout << "\n\n";

    if (!overwrite) path.replace_filename(path.stem().string() + "-min.png");
    writeFile(path, requiredChunks, requiredChunkNames);
    stripSizeTotal += std::filesystem::file_size(path);
}

int main(int argc, char** argv) try {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory> [r|o]\n";
        return 0;
    }

    bool recursive = argc >= 3 && std::string_view{ argv[2] }.contains('r');
    bool overwrite = argc >= 3 && std::string_view{ argv[2] }.contains('o');

    if (recursive)
        for (const auto& file : std::filesystem::recursive_directory_iterator{ argv[1] })
            stripFile(file, overwrite);
    else
        for (const auto& file : std::filesystem::directory_iterator{ argv[1] })
            stripFile(file, overwrite);

    std::cout << "Original: " << origSizeTotal << " B, Stripped: " << stripSizeTotal << " B\n";

    std::size_t diff = origSizeTotal - stripSizeTotal;
    std::cout << "Saved " << diff / static_cast<float>(origSizeTotal) * 100.0 << "% (" << diff << " B)\n";
    return 0;
} catch (const std::runtime_error& e) {
    std::cerr << e.what() << '\n';
    return 1;
}
