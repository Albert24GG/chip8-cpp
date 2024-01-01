#include "beeper.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <SDL.h>

namespace c8::beep {

// Audio callback function
static void audioCallback(void *userdata, Uint8 *stream, int len) {
    static double samplingIndex = 0;

    constexpr int sampleSize = sizeof(int16_t);
    auto *sampleBuffer       = reinterpret_cast<int16_t *>(stream);
    const int numSamples     = len / sampleSize;

    for (int i = 0; i < numSamples; ++i) {
        auto sample = static_cast<int16_t>(std::sin(samplingIndex) * amplitude);

        sampleBuffer[i] = sample;

        samplingIndex += 2 * std::numbers::pi * frequency / sampleRate;

        if (samplingIndex > 2 * std::numbers::pi) {
            samplingIndex -= 2 * std::numbers::pi;
        }
    }
}

SDL_AudioDeviceID generateBeeper() {

    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);

    audioSpec.freq     = sampleRate;
    audioSpec.format   = format;
    audioSpec.channels = channels;
    audioSpec.samples  = samples;
    audioSpec.callback = audioCallback;

    SDL_AudioDeviceID audioDevice{
        SDL_OpenAudioDevice(nullptr, 0, &audioSpec, nullptr, 0)};

    if (audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError()
                  << std::endl;
        return 0;
    }

    return audioDevice;
}
} // namespace c8::beep
