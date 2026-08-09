#include "switch_vfs.h"

#include <iostream>
#include <fstream>

SwitchVFS::SwitchVFS()
    : mounted(false)
{
}

bool SwitchVFS::Mount(const std::string& root)
{
    std::filesystem::path path(root);

    if (!std::filesystem::exists(path))
    {
        std::cout << "[VFS ERROR] Directory does not exist: "
                  << root << "\n";
        return false;
    }

    if (!std::filesystem::is_directory(path))
    {
        std::cout << "[VFS ERROR] Path is not a directory: "
                  << root << "\n";
        return false;
    }

    root_path = std::filesystem::absolute(path);
    mounted = true;

    std::cout << "[VFS] Mounted: "
              << root_path.string() << "\n";

    return true;
}

bool SwitchVFS::IsMounted() const
{
    return mounted;
}

bool SwitchVFS::Exists(const std::string& path) const
{
    if (!mounted)
        return false;

    std::filesystem::path full_path =
        root_path / std::filesystem::path(path);

    return std::filesystem::exists(full_path);
}

bool SwitchVFS::ReadFile(
    const std::string& path,
    std::vector<uint8_t>& data
) const
{
    if (!mounted)
        return false;

    std::filesystem::path full_path =
        root_path / std::filesystem::path(path);

    if (!std::filesystem::exists(full_path))
        return false;

    if (!std::filesystem::is_regular_file(full_path))
        return false;

    std::ifstream file(
        full_path,
        std::ios::binary | std::ios::ate
    );

    if (!file)
        return false;

    std::streamsize size = file.tellg();

    if (size < 0)
        return false;

    file.seekg(0, std::ios::beg);

    data.resize(static_cast<size_t>(size));

    if (size > 0)
    {
        file.read(
            reinterpret_cast<char*>(data.data()),
            size
        );
    }

    return file.good() || file.eof();
}

std::vector<std::string> SwitchVFS::ListDirectory(
    const std::string& path
) const
{
    std::vector<std::string> result;

    if (!mounted)
        return result;

    std::filesystem::path directory =
        root_path / std::filesystem::path(path);

    if (!std::filesystem::exists(directory))
        return result;

    if (!std::filesystem::is_directory(directory))
        return result;

    for (const auto& entry :
         std::filesystem::directory_iterator(directory))
    {
        result.push_back(entry.path().filename().string());
    }

    return result;
}