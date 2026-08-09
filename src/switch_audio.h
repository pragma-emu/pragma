#pragma once
#include <cstdint>
#include <string>
#include "miniaudio.h"

class SwitchAudioEngine {
private:
    ma_device device;
    ma_decoder decoder; 
    bool is_initialized;

public:
    SwitchAudioEngine();
    ~SwitchAudioEngine();

    // FIXED: Ensured this line is explicitly declared inside the class!
    void PlayMP3BackgroundMusic(const std::string& file_path);
    
    static void AudioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};
