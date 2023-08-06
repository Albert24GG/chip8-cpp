#include "chip8.hpp"

namespace c8 {

void Chip8::timerFunc() {
    while(!this->quit){
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16.67ms); // run at 60Hz
        --this->reg.DT;
        --this->reg.ST;
    }
}

uint16_t Chip8::getInstruction() {
    uint16_t ins =
        (this->ram[this->reg.PC] << 8) + this->ram[(this->reg.PC) + 1];
    this->reg.PC += 2;
    return ins;
}

void Chip8::drawSprite(const uint8_t xCoord, const uint8_t yCoord,
                       const uint8_t n) {
    std::vector<uint8_t> spriteVector{this->ram.begin() + this->reg.I,
                                      this->ram.begin() + this->reg.I + n + 1};

    this->reg.V[0xF] = 0;

    uint8_t limY = std::min(yCoord + n, 32);
    for (uint8_t row = yCoord; row < limY; ++row) {
        uint64_t sprite = spriteVector[row - yCoord];

        // check sprite's position and adjust it accordingly with the screen
        // limits
        if (xCoord + 8 <= 64)
            sprite <<= 64 - 8 - xCoord;
        else
            sprite >>= xCoord + 8 - 64;

        // set the Vf register to 1 if any pixel is erased from the screen
        this->reg.V[0xF] |= this->window.applySprite(row, sprite);
    }
}

void Chip8::decodeInstruction(uint16_t ins) {
    switch (ins >> 12) {
    case 0x0:
        if ((ins & 0xFF) == 0xEE) { // 00EE
            this->reg.PC = this->stack.top();
            this->stack.pop();
        } else
            this->clearScreen(); // OOE0
        break;

    case 0x1: // 1NNN jump to NNN
        this->reg.PC = (ins & 0xFFF);
        break;

    case 0x2: // 2NNN jump to NNN and push current PC to the stack
        this->stack.push(this->reg.PC);
        this->reg.PC = (ins & 0xFFF);
        break;

    case 0x3: // 3XNN skip next instruction if Vx == NN
        if (this->reg.V[(ins >> 8) & 0xF] == (ins & 0xFF))
            this->reg.PC += 2;
        break;

    case 0x4: // 4XNN skip next instruction if Vx != NN
        if (this->reg.V[(ins >> 8) & 0xF] != (ins & 0xFF))
            this->reg.PC += 2;
        break;

    case 0x5: // 5XY0 skip next instruction if Vx == Vy
        if (this->reg.V[(ins >> 8) & 0xF] == this->reg.V[(ins >> 4) & 0xF])
            this->reg.PC += 2;
        break;

    case 0x9: // 5XY0 skip next instruction if Vx != Vy
        if (this->reg.V[(ins >> 8) & 0xF] != this->reg.V[(ins >> 4) & 0xF])
            this->reg.PC += 2;
        break;

    case 0x6: // 6XNN set Vx to NN
        this->reg.V[(ins >> 8) & 0xF] =
            (ins & 0x0FF); // ins >> 8 is the second byte
        break;

    case 0x7: // 7XNN add NN to Vx
        this->reg.V[(ins >> 8) & 0xF] += (ins & 0x0FF);
        break;

    case 0x8: // 8XYK
        switch (ins & 0xF) {
        case 0x0: // 8XY0 set Vx to Vy
            this->reg.V[(ins >> 8) & 0xF] = this->reg.V[(ins >> 4) & 0xF];
            break;

        case 0x1: // 8XY1 Vx = Vx | Vy
            this->reg.V[(ins >> 8) & 0xF] |= this->reg.V[(ins >> 4) & 0xF];
            break;

        case 0x2: // 8XY2 Vx = Vx & Vy
            this->reg.V[(ins >> 8) & 0xF] &= this->reg.V[(ins >> 4) & 0xF];
            break;

        case 0x3: // 8XY3 Vx = Vx ^ Vy
            this->reg.V[(ins >> 8) & 0xF] ^= this->reg.V[(ins >> 4) & 0xF];
            break;

        case 0x4: { // 8XY4 Vx = Vx + Vy; Vf = 1 if result > 255, 0 otherwise
            bool carryFlag =
                ((static_cast<uint32_t>(this->reg.V[(ins >> 8) & 0xF]) +
                  this->reg.V[(ins >> 4) & 0xF]) > 255);
            this->reg.V[(ins >> 8) & 0xF] += this->reg.V[(ins >> 4) & 0xF];
            this->reg.V[0xF] = carryFlag;
            break;
        }

        case 0x5: { // 8XY5 Vx = Vx - Vy; Vf = 1 if Vx > Vy, 0 otherwise
            bool carryFlag =
                (this->reg.V[(ins >> 8) & 0xF] > this->reg.V[(ins >> 4) & 0xF]);
            this->reg.V[(ins >> 8) & 0xF] -= this->reg.V[(ins >> 4) & 0xF];
            this->reg.V[0xF] = carryFlag;
            break;
        }

        case 0x7: { // 8XY5 Vx = Vy - Vx; Vf = 1 if Vy > Vx, 0 otherwise
            bool carryFlag =
                (this->reg.V[(ins >> 8) & 0xF] < this->reg.V[(ins >> 4) & 0xF]);
            this->reg.V[(ins >> 8) & 0xF] =
                this->reg.V[(ins >> 4) & 0xF] - this->reg.V[(ins >> 8) & 0xF];
            this->reg.V[0xF] = carryFlag;
            break;
        }

        case 0x6: { // 8XY6 If the least-significant bit of Vx is 1, then VF is
                    // set to 1, otherwise 0. Then Vx is divided by 2
            bool carryFlag = (this->reg.V[(ins >> 8) & 0xF] & 1);
            this->reg.V[(ins >> 8) & 0xF] >>= 1;
            this->reg.V[0xF] = carryFlag;
            break;
        }

        case 0xE: { // 8XYE If the most-significant bit of Vx is 1, then VF is
                    // set to 1, otherwise to 0. Then Vx is multiplied by 2.
            bool carryFlag = ((this->reg.V[(ins >> 8) & 0xF] >> 7) & 1);
            this->reg.V[(ins >> 8) & 0xF] <<= 1;
            this->reg.V[0xF] = carryFlag;
            break;
        }
        }
        break;

    case 0xA: // ANNN set register I to NNN
        this->reg.I = (ins & 0xFFF);
        break;

    case 0xB: // BNNN jump to NNN + V0
        this->reg.PC = (ins & 0xFFF) + this->reg.V[0x0];
        break;

    case 0xC: // CXNN Vx = randNumb & NN
        this->reg.V[(ins >> 8) & 0xF] =
            c8::utils::getRandomNumber() & (ins & 0xFF);
        break;

    case 0xD: // DXYN
        this->drawSprite(this->reg.V[(ins >> 8) & 0xF],
                         this->reg.V[(ins >> 4) & 0xF], ins & 0xF);
        break;

    case 0xE: // EX9E & EXA1
        if ((ins & 0xFF) == 0x9E) { // skip if key coresponding to Vx is pressed
            bool isKeyDown = c8::keyboard::keyDown(this->getKeyInput(), this->reg.V[(ins >> 8) & 0xF]);
            if(isKeyDown){
                this->reg.PC += 2;
                //this->setKeyInput(0xFF);
            }
        } else {                    // skip if key coresponding to Vx is not pressed
            bool isKeyUp = c8::keyboard::keyUp(this->getKeyInput(), this->reg.V[(ins >> 8) & 0xF]);
            if(isKeyUp){
                this->reg.PC += 2;
                //this->setKeyInput(0xFF);
            }
        }
        break;
    case 0xF:
        switch (ins & 0xFF) {
        case 0x07: // FX07 Vx = DT
            this->reg.V[(ins >> 8) & 0xF] = this->reg.DT;
            break;

        case 0x15: // FX15 DT = Vx
            this->reg.DT = this->reg.V[(ins >> 8) & 0xF];
            break;

        case 0x18: // FX18 ST = Vx
            this->reg.ST = this->reg.V[(ins >> 8) & 0xF];
            break;

        case 0x1E: // FX1E I += Vx
            this->reg.I += this->reg.V[(ins >> 8) & 0xF];
            break;

        case 0x29: // FX29 point I to the address of the hexadecimal character in Vx
            this->reg.I = fontStart + 5 * (this->reg.V[(ins >> 8) & 0xF] & 0xF);
            break;

        case 0x0A:{// FX0A Vx = key pressed
            //uint8_t keyPressed = c8::keyboard::convertScanCode(c8::keyboard::getKeyPressed());
            using namespace c8::keyboard;
            if(keyValid(this->getKeyInput())){ // key is valid
                this->reg.V[(ins >> 8) & 0xF] = this->getKeyInput();
                //this->setKeyInput(0xFF);
            }
            else
                this->reg.PC -= 2;
            break;
        }

        case 0x33: // FX33 store the digits of decimal representation of Vx in
                   // the 3 consecutive bytes starting from address pointed by
                   // register I
            this->ram[this->reg.I] = (this->reg.V[(ins >> 8) & 0xF]) / 100;
            this->ram[this->reg.I + 1] =
                ((this->reg.V[(ins >> 8) & 0xF]) / 10) % 10;
            this->ram[this->reg.I + 2] = (this->reg.V[(ins >> 8) & 0xF]) % 10;
            break;

        case 0x55: // FX55 copy values V0...Vx in the ram at locations I...I+x
            for (uint8_t index = 0x0;
                 index <= static_cast<uint8_t>((ins >> 8) & 0xF); ++index) {
                this->ram[this->reg.I + index] = this->reg.V[index];
            }
            break;

        case 0x65: // FX65 read value V0...Vx from the ram at location I...I+x
            for (uint8_t index = 0x0;
                 index <= static_cast<uint8_t>((ins >> 8) & 0xF); ++index) {
                this->reg.V[index] = this->ram[this->reg.I + index];
            }
            break;
        }
        break;
    }
}
} // namespace c8