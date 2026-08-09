#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include "switch_gpu.h"
#include "switch_audio.h"
#include "switch_memory.h"
#include "switch_vfs.h"

namespace NvidiaSoC {
    const uint64_t PAGE_SIZE_4KB = 0x1000;
    const size_t MAX_CPU_CORES = 8; // The full 8-Core cluster footprint
    
    struct SMMUEntry {
        uint64_t virtual_page_address;
        uint64_t physical_memory_offset;
        bool is_executable;
        bool is_writable;
    };

    struct CoreContext {
        uint64_t registers[31]; // Comprehensive AArch64 X0 - X30 Registers
        uint64_t pc;
        uint64_t ticks;
        bool is_active;
    };
}

class SwitchCPU {
private:
    // FIXED: Put back the 8-Core tracker arrays that the compiler was missing!
    NvidiaSoC::CoreContext cores[NvidiaSoC::MAX_CPU_CORES];
    bool core_active_states[NvidiaSoC::MAX_CPU_CORES];
    
    std::vector<NvidiaSoC::SMMUEntry> smmu_table;
    std::mutex cluster_bus_mutex;
    std::vector<std::function<void(size_t)>> jit_block_cache;

public:
    SwitchMemory memory;
    SwitchGPU gpu; 
    SwitchAudioEngine audio; 
    SwitchVFS vfs;

    SwitchCPU();
    ~SwitchCPU() = default;

    bool LoadRomFSPackage(const std::string& file_path);
    bool LoadExecutableBinary(const std::string& binary_path);

    // Full 8-Core Symmetric Multiprocessing Engines
    void ExecuteCoreCluster(size_t core_id);
    void ActivateSecondaryCore(size_t core_id, uint64_t entry_point);
    
    // Comprehensive JIT Compilation Mock Injection Hook
    void InjectJITCompiledBlock(uint64_t vaddr, std::function<void(size_t)> compiled_block);

    // Register / State Controls
    void SetRegister(size_t core_id, size_t reg_index, uint64_t value);
    uint64_t GetRegister(size_t core_id, size_t reg_index);
    void SetPC(size_t core_id, uint64_t address);

    // Physical Memory Reading / Writing Matrix Handlers
    uint8_t MemoryRead8(uint64_t vaddr);
    uint32_t FetchInstruction(size_t core_id);
    uint32_t MemoryRead32(uint64_t vaddr);
    void MemoryWrite32(uint64_t vaddr, uint32_t value);
    
    // Core OS Interceptor
    void CallSVC(size_t core_id, uint32_t swi);
    uint64_t TranslateSoCAddress(uint64_t virtual_address);
    void MapSoCMemoryPage(uint64_t vaddr, uint64_t paddr, bool exec, bool write);
};
