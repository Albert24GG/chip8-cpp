#include "chip8.hpp"

namespace c8{
    uint16_t Chip8::getInstruction() {
        uint16_t ins = (this->ram[this->reg.PC] << 8) + this->ram[(this->reg.PC) + 1];
        this->reg.PC += 2;
        return ins;
    }

    void Chip8::decodeInstruction(uint16_t ins) {
        switch (ins >> 12) {
        case 0x0:
            this->clearScreen();
            break;
        case 0x1: // 1NNN jump to NNN
            this->reg.PC = (ins & 0xFFF);
            break;
        case 0x6: // 6XNN set Vx to NN
            this->reg.V[(ins >> 8) & 0xF] = (ins & 0x0FF); //ins >> 8 is the second byte
            break;
        case 0x7: // 7XNN add NN to Vx
            this->reg.V[(ins >> 8) & 0xF] += (ins & 0x0FF);
            break;
        case 0xA: // ANNN set register I to NNN
            this->reg.I = (ins & 0xFFF);
            break;
        case 0xD: // DXYN
            this->drawSprite(this->reg.V[(ins >> 8) & 0xF], this->reg.V[(ins >> 4) & 0xF], ins & 0xF);
        }
    }

    void Chip8::drawSprite(const uint8_t xCoord, const uint8_t yCoord, const uint8_t n){
        std::vector<uint8_t> spriteVector{this->ram.begin() + this->reg.I, this->ram.begin() + this->reg.I + n + 1};

        this->reg.V[0xF] = 0;

        uint8_t limY = std::min(yCoord + n, 64);
        for(uint8_t row = yCoord; row < limY; ++row){
            uint64_t sprite = spriteVector[row - yCoord];

            // check sprite's position and adjust it accordingly with the screen limits
            if(xCoord + 8 <= 64) sprite <<= 64 - 8 - xCoord;
            else sprite >>= xCoord + 8 - 64;

            // set the Vf register to 1 if any pixel is erased from the screen
            this->reg.V[0xF] |= this->window.applySprite(row, sprite);

        }
    }
}
