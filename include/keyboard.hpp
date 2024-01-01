#pragma once

#include <cstdint>
#include <SDL.h>

namespace c8::keyboard {

uint8_t convertScanCodeToKey(SDL_Scancode code);

inline bool keyValid(uint8_t key) { return key != 0xFF; }

inline bool keyDown(uint8_t keyInput, uint8_t key) { return keyInput == key; }

inline bool keyUp(uint8_t keyInput, uint8_t key) {
    return !keyDown(keyInput, key);
}

} // namespace c8::keyboard
