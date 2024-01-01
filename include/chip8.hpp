#pragma once

#include "display.hpp"
#include "keyboard.hpp"
#include "stack.hpp"
#include "utils.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <thread>
#include <vector>

namespace c8 {

enum AddressSpace : uint16_t {
    reservedStart = 0x000,
    reservedEnd   = 0x1FF,
    programStart  = 0x200,
    programEnd    = 0xFFF,
    fontStart     = 0x050,
    fontEnd       = 0x09F
};

struct Registers {
    uint16_t PC{programStart};
    uint16_t I{};
    uint8_t SP{};
    uint8_t DT{}, ST{};
    std::array<uint8_t, 16> V{{}};
};

inline constexpr std::array<uint8_t, 80> font{{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
}};

class Chip8 {
    public:
    Chip8();
    ~Chip8();
    Chip8(const Chip8 &)            = delete;
    Chip8 &operator=(const Chip8 &) = delete;
    Chip8(Chip8 &&)                 = delete;
    Chip8 &operator=(Chip8 &&)      = delete;

    using PathType = std::filesystem::path;
    friend bool c8::utils::readProgramToMemory(Chip8 &chip8,
        const PathType &path);
    friend void c8::utils::loadFontToMemory(Chip8 &chip8,
        const std::array<uint8_t, 80> &font);

    friend void soundThreadFunction(Chip8 &chip8);

    uint16_t getInstruction();
    void decodeInstruction(uint16_t ins);

    inline void clearScreen() { window.resetScreen(); }

    inline void renderScreen() { window.renderScreen(); }

    void drawSprite(uint8_t xCoord, uint8_t yCoord, uint8_t n);

    [[nodiscard]] inline uint8_t getKeyInput() const { return keyInput; }

    inline void setKeyInput(uint8_t keyInput) { this->keyInput = keyInput; }

    inline void updateScreen(bool shouldRender) {
        this->shouldRender = shouldRender;
    }

    [[nodiscard]] inline bool shouldUpdateScreen() const {
        return shouldRender;
    }

    std::atomic_bool quit{false};


    private:
    Registers reg{};
    std::array<uint8_t, 4096> ram{{}}; // 4KB of ram
    Stack<uint16_t, 16> stack{Stack<uint16_t, 16>{reg.SP}};
    c8::display::Window window{};
    std::atomic_uint8_t keyInput{0xFF};
    std::thread timer;
    bool shouldRender{false};
};

} // namespace c8
