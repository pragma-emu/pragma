#pragma once

#include <string>

class SwitchGameLoader
{
public:
    void ScanDirectory(const std::string& directory);

private:
    bool LoadXCI(const std::string& path);
    bool LoadNSO(const std::string& path);

    void InspectFile(const std::string& type,
                     const std::string& path);
};