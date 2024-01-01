#include "keyboard.hpp"

#include "chip8.hpp"
#include "SDL.h"

#include <thread>

namespace c8::keyboard {

uint8_t convertScanCodeToKey(SDL_Scancode code) {
    switch (code) {
    case SDL_SCANCODE_X:
        return 0x0;
    case SDL_SCANCODE_1:
        return 0x1;
    case SDL_SCANCODE_2:
        return 0x2;
    case SDL_SCANCODE_3:
        return 0x3;
    case SDL_SCANCODE_Q:
        return 0x4;
    case SDL_SCANCODE_W:
        return 0x5;
    case SDL_SCANCODE_E:
        return 0x6;
    case SDL_SCANCODE_A:
        return 0x7;
    case SDL_SCANCODE_S:
        return 0x8;
    case SDL_SCANCODE_D:
        return 0x9;
    case SDL_SCANCODE_Z:
        return 0xA;
    case SDL_SCANCODE_C:
        return 0xB;
    case SDL_SCANCODE_4:
        return 0xC;
    case SDL_SCANCODE_R:
        return 0xD;
    case SDL_SCANCODE_F:
        return 0xE;
    case SDL_SCANCODE_V:
        return 0xF;
    default:
        return 0xFF;
    }
}

void handleInput(c8::Chip8 &chip8) {
    SDL_Event event;

    while (!chip8.quit) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT:
                chip8.quit = true;
                break;

            case SDL_KEYDOWN:
                {
                    using namespace c8::keyboard;
                    uint8_t keyPressed =
                        convertScanCodeToKey(event.key.keysym.scancode);
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        chip8.quit = true;
                    else if (keyValid(keyPressed)) {
                        chip8.setKeyInput(keyPressed);
                    }
                    break;
                }

            case SDL_KEYUP:
                chip8.setKeyInput(0xFF);
                break;
            }
        }
        std::this_thread::sleep_for(
            std::chrono::microseconds(100)); // Optional: Reduce CPU usage
    }
};

} // namespace c8::keyboard
