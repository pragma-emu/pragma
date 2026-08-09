#define MINIAUDIO_IMPLEMENTATION
#include "switch_audio.h"
#include <iostream>

SwitchAudioEngine::SwitchAudioEngine() : is_initialized(false) {}

SwitchAudioEngine::~SwitchAudioEngine() {
    if (is_initialized) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }
}

// Miniaudio calls this whenever the sound card needs more speaker samples
void SwitchAudioEngine::AudioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = static_cast<ma_decoder*>(pDevice->pUserData);
    if (pDecoder == nullptr) return;

    // Read and decode the exact number of frames required straight out of the MP3 file!
    ma_uint64 framesRead;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);
    
    // Smoothly loop back to the start of the audio file if the music finishes playing
    if (framesRead < frameCount) {
        ma_decoder_seek_to_pcm_frame(pDecoder, 0);
    }
}

void SwitchAudioEngine::PlayMP3BackgroundMusic(const std::string& file_path) {
    // 1. Initialize the built-in MP3 decoding pipeline
    ma_result result = ma_decoder_init_file(file_path.c_str(), NULL, &decoder);
    if (result != MA_SUCCESS) {
        std::cout << "[SWITCH 2 APU ERROR] Failed to load or decode MP3 file: " << file_path << "\n";
        return;
    }

    // 2. Align your device configurations to match the file's natural sample rates and channels
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = AudioCallback;
    deviceConfig.pUserData         = &decoder; // Pass the decoder tracking context over to the callback loop

    // 3. Mount the physical desktop hardware output device
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "[SWITCH 2 APU ERROR] Failed to mount host audio device properties.\n";
        ma_decoder_uninit(&decoder);
        return;
    }

    // 4. Activate audio playback
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "[SWITCH 2 APU ERROR] Failed to initialize hardware playback channels.\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    is_initialized = true;
    std::cout << "[SWITCH 2 APU AUDREN] Background Music Active! Streaming: " << file_path << "\n";
    std::cout << "  Audio Output Profile -> Channels: " << decoder.outputChannels 
              << " | Frequency: " << decoder.outputSampleRate << "Hz\n\n";
}
