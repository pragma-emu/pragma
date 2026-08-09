#include "switch_cpu.h"
#include "switch_game_loader.h"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    SwitchCPU cpu;
    SwitchGameLoader loader;

    loader.ScanDirectory("Games");
    
cpu.memory.write32(0x1000, 0xD503201F);

cpu.SetPC(0, 0x1000);

uint32_t instruction = cpu.FetchInstruction(0);

    std::cout << "========================================================\n";
    std::cout << "                    PRAGMA VFS TEST\n";
    std::cout << "========================================================\n\n";

    // Mount the Games folder
    if (!cpu.vfs.Mount("Games"))
    {
        std::cout << "[VFS TEST] FAILED TO MOUNT Games/\n";
        return 1;
    }

    std::cout << "\n[VFS] Games directory:\n";

    // List files
    auto files = cpu.vfs.ListDirectory("/");

    for (const auto& file : files)
    {
        std::cout << "  " << file << "\n";
    }

    // Test file
    const std::string test_file = "test.txt";

    if (!cpu.vfs.Exists(test_file))
    {
        std::cout << "\n[VFS TEST] test.txt was not found!\n";
        return 1;
    }

    std::cout << "\n[VFS] Found: " << test_file << "\n";

    // Read the file
    std::vector<uint8_t> data;

    if (!cpu.vfs.ReadFile(test_file, data))
    {
        std::cout << "[VFS TEST] Failed to read test.txt!\n";
        return 1;
    }

    std::cout << "[VFS] Read " << data.size() << " bytes\n";
    std::cout << "[VFS] Content: ";

    for (uint8_t byte : data)
    {
        std::cout << static_cast<char>(byte);
    }

    std::cout << "\n\n";
    std::cout << "[VFS TEST] PASS!\n";

    std::cout << "========================================================\n";

    return 0;
}