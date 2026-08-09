#pragma once

#include <cstdint>
#include <iostream>

#include "switch_memory.h"

class SwitchGPU {
public:
    static const int SCREEN_WIDTH = 4;
    static const int SCREEN_HEIGHT = 4;

    static const uint64_t FRAMEBUFFER_VADDR = 0x50000;

    SwitchGPU() = default;
    ~SwitchGPU() = default;

    void RenderFramebuffer(SwitchMemory& memory);
};