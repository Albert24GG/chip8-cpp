#include "chip8.hpp"
#include "keyboard.hpp"
#include "utils.hpp"

#include <filesystem>
#include <iostream>
#include <SDL.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: Please provide only 1 argument(path to the rom)!";
        return 1;
    }

    std::filesystem::path romPath{argv[1]};
    if (!std::filesystem::exists(romPath)) {
        std::cerr << "ERROR: Path provided does not existe";
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
                  << std::endl;
        return 1;
    }

    c8::Chip8 chip8{};

    if (!c8::utils::readProgramToMemory(chip8, romPath))
        return 1;

    c8::utils::loadFontToMemory(chip8, c8::font);

    std::thread inputThread(c8::keyboard::handleInput, std::ref(chip8));

    while (!chip8.quit) {
        chip8.decodeInstruction(chip8.getInstruction());

        if (chip8.shouldUpdateScreen()) {
            chip8.renderScreen();
            chip8.updateScreen(false);
        }

        SDL_Delay(2);
    }

    inputThread.join();

    return 0;
}
