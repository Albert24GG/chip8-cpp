#include "utils.hpp"
#include "chip8.hpp"

namespace c8::utils {

bool readProgramToMemory(c8::Chip8 &chip8, const PathType &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "ERROR: Could not open file at the specified path.";
    return false;
  }

  file.seekg(0, std::ios::end);
  auto fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  if (fileSize > c8::programEnd - c8::programStart) {
    std::cerr << "ERROR: File provided is too large.";
    return false;
  }

  file.read(std::bit_cast<char *>(chip8.ram.data()) + c8::programStart,
            fileSize);
  file.close();

  return true;
}

void loadFontToMemory(c8::Chip8 &chip8, const std::array<uint8_t, 80> &font) {
  std::copy(font.begin(), font.end(), chip8.ram.begin() + c8::fontStart);
}

uint8_t getRandomNumber(){
  std::random_device rd;   // Seed the generator with a truly random value
  std::mt19937 rng(rd());  // Mersenne Twister engine

  std::uniform_int_distribution<int> distribution(0x0, 0xFF);

  // Generate a random number within the defined range
  return distribution(rng);
}

} // namespace c8::utils
