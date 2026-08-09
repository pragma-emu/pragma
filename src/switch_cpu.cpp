#include "switch_cpu.h"

#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>

// ============================================================
// SwitchCPU Constructor
// ============================================================

SwitchCPU::SwitchCPU()
{
    for (size_t i = 0; i < NvidiaSoC::MAX_CPU_CORES; ++i)
    {
        std::memset(
            cores[i].registers,
            0,
            sizeof(cores[i].registers)
        );

        cores[i].pc = 0;
        cores[i].ticks = 0;
        cores[i].is_active = (i == 0);
    }
}
// ============================================================
// JIT Block Injection
// ============================================================

void SwitchCPU::InjectJITCompiledBlock(
    uint64_t vaddr,
    std::function<void(size_t)> compiled_block)
{
    std::lock_guard<std::mutex> lock(cluster_bus_mutex);

    if (vaddr >= jit_block_cache.size())
    {
        std::cout << "[PRAGMA JIT ERROR] Address outside JIT cache: 0x"
                  << std::hex << vaddr << std::dec << "\n";
        return;
    }

    jit_block_cache[vaddr] = compiled_block;
}

// ============================================================
// Execute CPU Core
// ============================================================

void SwitchCPU::ExecuteCoreCluster(size_t core_id)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return;

    if (!cores[core_id].is_active)
        return;

    uint64_t current_pc = cores[core_id].pc;

    if (current_pc >= jit_block_cache.size())
    {
        std::cout
            << "[CORE " << core_id
            << " ERROR] PC outside JIT block cache: 0x"
            << std::hex << current_pc
            << std::dec << "\n";

        return;
    }

    if (jit_block_cache[current_pc] != nullptr)
    {
        jit_block_cache[current_pc](core_id);
    }
    else
    {
        std::cout
            << "[CORE " << core_id
            << " ERROR] Hit uncompiled memory address: 0x"
            << std::hex << current_pc
            << std::dec << "\n";
    }
}

// ============================================================
// Activate Secondary CPU Core
// ============================================================

void SwitchCPU::ActivateSecondaryCore(
    size_t core_id,
    uint64_t entry_point)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return;

    cores[core_id].is_active = true;
    cores[core_id].pc = entry_point;

    std::thread hardware_core_thread(
        [this, core_id]()
        {
            std::cout
                << "[NVIDIA CPU CLUSTER] PHYSICAL CORE ["
                << core_id
                << "] AWAKENED!\n";

            ExecuteCoreCluster(core_id);
        }
    );

    hardware_core_thread.detach();
}

// ============================================================
// SMMU / Memory Mapping
// ============================================================

void SwitchCPU::MapSoCMemoryPage(
    uint64_t vaddr,
    uint64_t paddr,
    bool exec,
    bool write)
{
    std::lock_guard<std::mutex> lock(cluster_bus_mutex);

    NvidiaSoC::SMMUEntry entry;

    entry.virtual_page_address =
        vaddr & ~(NvidiaSoC::PAGE_SIZE_4KB - 1);

    entry.physical_memory_offset =
        paddr & ~(NvidiaSoC::PAGE_SIZE_4KB - 1);

    entry.is_executable = exec;
    entry.is_writable = write;

    smmu_table.push_back(entry);
}

// ============================================================
// Virtual → Physical Address Translation
// ============================================================

uint64_t SwitchCPU::TranslateSoCAddress(
    uint64_t virtual_address)
{
    uint64_t page_base =
        virtual_address &
        ~(NvidiaSoC::PAGE_SIZE_4KB - 1);

    uint64_t page_offset =
        virtual_address &
        (NvidiaSoC::PAGE_SIZE_4KB - 1);

    for (const auto& entry : smmu_table)
    {
        if (entry.virtual_page_address == page_base)
        {
            return entry.physical_memory_offset +
                   page_offset;
        }
    }

    // Identity mapping when no SMMU mapping exists.
    return virtual_address;
}

// ============================================================
// Memory Read / Write
// ============================================================

uint8_t SwitchCPU::MemoryRead8(uint64_t vaddr)
{
    std::lock_guard<std::mutex> lock(cluster_bus_mutex);

    uint64_t physical_address =
        TranslateSoCAddress(vaddr);

    return memory.read8(physical_address);
}

uint32_t SwitchCPU::FetchInstruction(size_t core_id)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return 0;

    uint64_t pc = cores[core_id].pc;

    uint32_t instruction = memory.read32(pc);

    std::cout << "[CPU] Core " << core_id
              << " FETCH PC=0x" << std::hex << pc
              << " INSTRUCTION=0x" << instruction
              << std::dec << "\n";

    return instruction;
}

uint32_t SwitchCPU::MemoryRead32(uint64_t vaddr)
{
    std::lock_guard<std::mutex> lock(cluster_bus_mutex);

    uint64_t physical_address =
        TranslateSoCAddress(vaddr);

    return memory.read32(physical_address);
}

void SwitchCPU::MemoryWrite32(
    uint64_t vaddr,
    uint32_t value)
{
    std::lock_guard<std::mutex> lock(cluster_bus_mutex);

    uint64_t physical_address =
        TranslateSoCAddress(vaddr);

    memory.write32(
        physical_address,
        value
    );
}

// ============================================================
// CPU Registers
// ============================================================

void SwitchCPU::SetRegister(
    size_t core_id,
    size_t reg_index,
    uint64_t value)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return;

    if (reg_index >= 31)
        return;

    cores[core_id].registers[reg_index] = value;
}

uint64_t SwitchCPU::GetRegister(
    size_t core_id,
    size_t reg_index)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return 0;

    if (reg_index >= 31)
        return 0;

    return cores[core_id].registers[reg_index];
}

// ============================================================
// Program Counter
// ============================================================

void SwitchCPU::SetPC(
    size_t core_id,
    uint64_t address)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return;

    cores[core_id].pc = address;
}

// ============================================================
// SVC / System Call Handler
// ============================================================

void SwitchCPU::CallSVC(
    size_t core_id,
    uint32_t swi)
{
    if (core_id >= NvidiaSoC::MAX_CPU_CORES)
        return;

    uint64_t svc_id =
        cores[core_id].registers[8];

    if (svc_id == 0x08)
    {
        // CreateThread
        uint64_t target_address =
            cores[core_id].registers[0];

        size_t next_available_core = 1;

        std::cout
            << "\n[CORE " << core_id
            << " HLE OS] Intercepted CreateThread "
            << "System Call (SVC 0x08)\n";

        SetRegister(
            next_available_core,
            0,
            500
        );

        ActivateSecondaryCore(
            next_available_core,
            target_address
        );
    }

    cores[core_id].pc += 4;
}

// ============================================================
// ROMFS / Executable Loading
// ============================================================

bool SwitchCPU::LoadRomFSPackage(
    const std::string& file_path)
{
    std::cout
        << "[PRAGMA ROMFS] Loading: "
        << file_path << "\n";

    // TODO:
    // Implement actual ROMFS parsing.

    return true;
}

bool SwitchCPU::LoadExecutableBinary(
    const std::string& binary_path)
{
    std::cout
        << "[PRAGMA LOADER] Loading executable: "
        << binary_path << "\n";

    // TODO:
    // Implement executable loading.

    return true;
}