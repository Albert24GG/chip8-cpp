#include "chip8.hpp"
#include "utils.hpp"
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

  // std::cout << chip8.ram[512] << chip8.ram[513] << chip8.ram[514]
  //           << chip8.ram[515] << '\n';

  c8::utils::loadFontToMemory(chip8, c8::font);

  // std::cout << std::hex << static_cast<int>(chip8.ram[c8::fontEnd + 1]);
  //  std::cout << std::hex << static_cast<int>(chip8.ram[512] & 0xF);

  return 0;
}
