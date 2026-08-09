#include "switch_gpu.h"

void SwitchGPU::RenderFramebuffer(SwitchMemory& memory)
{
    std::cout << "\n---------------- GPU FRAMEBUFFER MATRIX ----------------\n";

    uint64_t current_pixel_address = FRAMEBUFFER_VADDR;

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        std::cout << "  ";

        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            uint8_t r = memory.read8(current_pixel_address);
            uint8_t g = memory.read8(current_pixel_address + 1);
            uint8_t b = memory.read8(current_pixel_address + 2);

            if (r == 0xFF && g == 0x00 && b == 0x00)
            {
                std::cout << "[🟥] ";
            }
            else if (r == 0x00 && g == 0xFF && b == 0x00)
            {
                std::cout << "[🟩] ";
            }
            else if (r == 0x00 && g == 0x00 && b == 0xFF)
            {
                std::cout << "[🟦] ";
            }
            else
            {
                std::cout << "[⬛] ";
            }

            current_pixel_address += 4;
        }

        std::cout << "\n";
    }

    std::cout << "--------------------------------------------------------\n\n";
}