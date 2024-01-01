#pragma once

#include <cstdint>
#include <SDL.h>

namespace c8::beep {

constexpr int sampleRate         = 44'100;
constexpr SDL_AudioFormat format = AUDIO_S16SYS;
constexpr Uint8 channels         = 1;
constexpr Uint16 samples         = 1024;
constexpr int frequency          = 440;
constexpr int16_t amplitude      = 32'000;


SDL_AudioDeviceID generateBeeper();

} // namespace c8::beep
