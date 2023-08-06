#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>

namespace c8 {

class Chip8;

}

namespace c8::utils {

using PathType = std::filesystem::path;

bool readProgramToMemory(c8::Chip8 &chip8, const PathType &path);

void loadFontToMemory(c8::Chip8 &chip8, const std::array<uint8_t, 80> &font);

uint8_t getRandomNumber();

} // namespace c8::utils
