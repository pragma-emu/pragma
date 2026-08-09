#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

class SwitchVFS
{
public:
    SwitchVFS();

    bool Mount(const std::string& root);

    bool IsMounted() const;

    bool Exists(const std::string& path) const;

    bool ReadFile(
        const std::string& path,
        std::vector<uint8_t>& data
    ) const;

    std::vector<std::string> ListDirectory(
        const std::string& path
    ) const;

private:
    std::filesystem::path root_path;
    bool mounted;
};