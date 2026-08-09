#include "switch_game_loader.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>

void SwitchGameLoader::ScanDirectory(const std::string& directory)
{
    if (!std::filesystem::exists(directory))
    {
        std::cout << "[GAME LOADER] Directory not found: "
                  << directory << "\n";
        return;
    }

    std::cout << "[GAME LOADER] Scanning: "
              << directory << "\n";

    for (const auto& entry :
         std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
            continue;

        std::string extension =
            entry.path().extension().string();

        if (extension == ".xci" || extension == ".XCI")
        {
            LoadXCI(entry.path().string());
        }
        else if (extension == ".nso" || extension == ".NSO")
        {
            LoadNSO(entry.path().string());
        }
    }
}

bool SwitchGameLoader::LoadXCI(const std::string& path)
{
    InspectFile("XCI", path);
    return true;
}

bool SwitchGameLoader::LoadNSO(const std::string& path)
{
    InspectFile("NSO", path);
    return true;
}

void SwitchGameLoader::InspectFile(
    const std::string& type,
    const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cout << "[" << type << "] Failed to open: "
                  << path << "\n";
        return;
    }

    std::streamsize size = file.tellg();

    file.seekg(0, std::ios::beg);

    std::cout << "[" << type << "] Found: "
              << path << "\n";

    std::cout << "[" << type << "] Size: "
              << size << " bytes\n";

    std::cout << "[" << type << "] First bytes: ";

    unsigned char buffer[16] = {};

    file.read(
        reinterpret_cast<char*>(buffer),
        sizeof(buffer)
    );

    std::streamsize bytes_read = file.gcount();

    for (std::streamsize i = 0; i < bytes_read; ++i)
    {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(buffer[i])
                  << " ";
    }

    std::cout << std::dec << "\n";
}