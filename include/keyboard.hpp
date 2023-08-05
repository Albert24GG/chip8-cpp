#pragma once

#include "chip8.hpp"
#include <SDL.h>
#include <array>
#include <chrono>
#include <thread>


namespace c8::keyboard {

uint8_t convertScanCodeToKey(SDL_Scancode code);

SDL_Scancode convertKeyToScanCode(uint8_t key);

SDL_Scancode getKeyPressed();

inline bool keyValid(uint8_t key){
    return key != 0xFF;
}

inline bool keyDown(uint8_t keyInput, uint8_t key){
    return keyInput == key;
    //const Uint8* currentKeyStates {SDL_GetKeyboardState(nullptr)};
    //return currentKeyStates[convertKeyToScanCode(key)];
}

inline bool keyUp(uint8_t keyInput, uint8_t key){
    return !keyDown(keyInput, key);
}

}