#pragma once

#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include <cstring>

class SwitchMemory {
public:
    static constexpr uint64_t RAM_SIZE =
        12ULL * 1024ULL * 1024ULL * 1024ULL;

    static constexpr uint64_t PAGE_SIZE = 0x1000;

    SwitchMemory() = default;
    ~SwitchMemory() = default;

    uint8_t read8(uint64_t address);
    uint32_t read32(uint64_t address);
    uint64_t read64(uint64_t address);

    void write8(uint64_t address, uint8_t value);
    void write32(uint64_t address, uint32_t value);
    void write64(uint64_t address, uint64_t value);

    uint8_t* get_pointer(uint64_t address);

    size_t allocated_pages() const {
        return pages.size();
    }

private:
    std::unordered_map<uint64_t, std::unique_ptr<uint8_t[]>> pages;

    uint8_t* get_page(uint64_t address);
};