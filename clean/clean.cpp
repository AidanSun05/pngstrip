#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <directory>\n";
        return 0;
    }

    for (const auto& file : std::filesystem::recursive_directory_iterator{ argv[1] })
        if (file.path().filename().string().ends_with("-min.png"))
            std::filesystem::remove(file);
}
