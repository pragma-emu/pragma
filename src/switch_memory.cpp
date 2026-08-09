#include "switch_memory.h"

uint8_t* SwitchMemory::get_page(uint64_t address)
{
    if (address >= RAM_SIZE)
        return nullptr;

    uint64_t page_number = address / PAGE_SIZE;

    auto it = pages.find(page_number);

    if (it == pages.end())
    {
        auto page = std::make_unique<uint8_t[]>(PAGE_SIZE);

        std::memset(page.get(), 0, PAGE_SIZE);

        uint8_t* ptr = page.get();

        pages.emplace(page_number, std::move(page));

        return ptr;
    }

    return it->second.get();
}

uint8_t* SwitchMemory::get_pointer(uint64_t address)
{
    uint8_t* page = get_page(address);

    if (!page)
        return nullptr;

    return page + (address & (PAGE_SIZE - 1));
}

uint8_t SwitchMemory::read8(uint64_t address)
{
    uint8_t* ptr = get_pointer(address);

    return ptr ? *ptr : 0;
}

uint32_t SwitchMemory::read32(uint64_t address)
{
    uint32_t value = 0;

    for (int i = 0; i < 4; ++i)
        value |= static_cast<uint32_t>(read8(address + i)) << (i * 8);

    return value;
}

uint64_t SwitchMemory::read64(uint64_t address)
{
    uint64_t value = 0;

    for (int i = 0; i < 8; ++i)
        value |= static_cast<uint64_t>(read8(address + i)) << (i * 8);

    return value;
}

void SwitchMemory::write8(uint64_t address, uint8_t value)
{
    uint8_t* ptr = get_pointer(address);

    if (ptr)
        *ptr = value;
}

void SwitchMemory::write32(uint64_t address, uint32_t value)
{
    for (int i = 0; i < 4; ++i)
        write8(address + i, (value >> (i * 8)) & 0xFF);
}

void SwitchMemory::write64(uint64_t address, uint64_t value)
{
    for (int i = 0; i < 8; ++i)
        write8(address + i, (value >> (i * 8)) & 0xFF);
}