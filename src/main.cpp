#include "chip8.hpp"
#include "utils.hpp"
#include "keyboard.hpp"
#include <SDL2/SDL.h>
#include <filesystem>
#include <iostream>

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


  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  c8::Chip8 chip8{};

  if (!c8::utils::readProgramToMemory(chip8, romPath))
    return 1;

  c8::utils::loadFontToMemory(chip8, c8::font);

  //bool quit = false;
  SDL_Event event;

  while (!chip8.quit) {
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
        case SDL_QUIT:
            chip8.quit = true;
            break;

        case SDL_KEYDOWN: {
            using namespace c8::keyboard;
            uint8_t keyPressed =
                convertScanCodeToKey(event.key.keysym.scancode);
            if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                chip8.quit = true;
            else if (keyValid(keyPressed)) {
                chip8.setKeyInput(keyPressed);
            }
            break;
        }

        case SDL_KEYUP:
            chip8.setKeyInput(0xFF);

        }
    }
        chip8.decodeInstruction(chip8.getInstruction());
        chip8.renderScreen();
        SDL_Delay(2);
  }
  return 0;
}
