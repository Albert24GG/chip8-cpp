#include "chip8.hpp"

namespace c8 {

Chip8::Chip8()
    : stack{Stack<uint16_t, 16>{reg.SP}}, timer{[this]() {
          // opening an audio device:
          SDL_AudioSpec audioSpec;
          SDL_zero(audioSpec);
          audioSpec.freq     = 44'100;
          audioSpec.format   = AUDIO_S16SYS;
          audioSpec.channels = 1;
          audioSpec.samples  = 1024;
          audioSpec.callback = NULL;

          // the representation of our audio device in SDL:
          SDL_AudioDeviceID audioDevice{
              SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0)};

          bool audioQueued{false};
          while (!this->quit) {
              if (this->reg.ST > 0 && !audioQueued) {
                  float x = 0;
                  for (int i = 0; i < audioSpec.freq * this->reg.ST / 60; i++) {
                      x += .050f;

                      int16_t sample = sin(x * 4) * 5000;

                      const int sample_size = sizeof(int16_t) * 1;
                      SDL_QueueAudio(audioDevice, &sample, sample_size);
                  }
                  // unpausing the audio device (starts playing):
                  SDL_PauseAudioDevice(audioDevice, 0);
                  audioQueued = true;
              }
              audioQueued = (this->reg.ST
                  != 0); // stop playing audio when timer reaches 0

              using namespace std::chrono_literals;
              std::this_thread::sleep_for(
                  16.67ms); // decrease timer at a rate of 60Hz
              this->reg.DT = std::max(0, this->reg.DT - 1);
              this->reg.ST = std::max(0, this->reg.ST - 1);
          }
          SDL_CloseAudioDevice(audioDevice);
      }} {}

Chip8::~Chip8() { timer.join(); }

uint16_t Chip8::getInstruction() {
    uint16_t ins = (ram[reg.PC] << 8) + ram[(reg.PC) + 1];
    reg.PC += 2;
    return ins;
}

void Chip8::drawSprite(uint8_t xCoord, uint8_t yCoord, uint8_t n) {

    std::vector<uint8_t> spriteVector{ram.begin() + reg.I,
        ram.begin() + reg.I + n + 1};

    reg.V[0xF] = 0;
    xCoord %= 64;
    yCoord %= 32;

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
        reg.V[0xF] |= window.applySprite(row, sprite);
    }
}

void Chip8::decodeInstruction(uint16_t ins) {
    switch (ins >> 12) {
    case 0x0:
        if ((ins & 0xFF) == 0xEE) { // 00EE
            reg.PC = stack.top();
            stack.pop();
        } else
            clearScreen(); // OOE0
        break;

    case 0x1: // 1NNN jump to NNN
        reg.PC = (ins & 0xFFF);
        break;

    case 0x2: // 2NNN jump to NNN and push current PC to the stack
        stack.push(reg.PC);
        reg.PC = (ins & 0xFFF);
        break;

    case 0x3: // 3XNN skip next instruction if Vx == NN
        if (reg.V[(ins >> 8) & 0xF] == (ins & 0xFF))
            reg.PC += 2;
        break;

    case 0x4: // 4XNN skip next instruction if Vx != NN
        if (reg.V[(ins >> 8) & 0xF] != (ins & 0xFF))
            reg.PC += 2;
        break;

    case 0x5: // 5XY0 skip next instruction if Vx == Vy
        if (reg.V[(ins >> 8) & 0xF] == reg.V[(ins >> 4) & 0xF])
            reg.PC += 2;
        break;

    case 0x9: // 5XY0 skip next instruction if Vx != Vy
        if (reg.V[(ins >> 8) & 0xF] != reg.V[(ins >> 4) & 0xF])
            reg.PC += 2;
        break;

    case 0x6:                                    // 6XNN set Vx to NN
        reg.V[(ins >> 8) & 0xF] = (ins & 0x0FF); // ins >> 8 is the second byte
        break;

    case 0x7: // 7XNN add NN to Vx
        reg.V[(ins >> 8) & 0xF] += (ins & 0x0FF);
        break;

    case 0x8: // 8XYK
        switch (ins & 0xF) {
        case 0x0: // 8XY0 set Vx to Vy
            reg.V[(ins >> 8) & 0xF] = reg.V[(ins >> 4) & 0xF];
            break;

        case 0x1: // 8XY1 Vx = Vx | Vy
            reg.V[0xF] = 0;
            reg.V[(ins >> 8) & 0xF] |= reg.V[(ins >> 4) & 0xF];
            break;

        case 0x2: // 8XY2 Vx = Vx & Vy
            reg.V[0xF] = 0;
            reg.V[(ins >> 8) & 0xF] &= reg.V[(ins >> 4) & 0xF];
            break;

        case 0x3: // 8XY3 Vx = Vx ^ Vy
            reg.V[0xF] = 0;
            reg.V[(ins >> 8) & 0xF] ^= reg.V[(ins >> 4) & 0xF];
            break;

        case 0x4: // 8XY4 Vx = Vx + Vy; Vf = 1 if result > 255, 0 otherwise
            // Set the carry flag
            reg.V[0xF] = ((static_cast<uint32_t>(reg.V[(ins >> 8) & 0xF])
                              + reg.V[(ins >> 4) & 0xF])
                > 255);
            reg.V[(ins >> 8) & 0xF] += reg.V[(ins >> 4) & 0xF];
            break;

        case 0x5: // 8XY5 Vx = Vx - Vy; Vf = 1 if Vx > Vy, 0 otherwise
            // Set the carry flag
            reg.V[0xF] = (reg.V[(ins >> 8) & 0xF] > reg.V[(ins >> 4) & 0xF]);
            reg.V[(ins >> 8) & 0xF] -= reg.V[(ins >> 4) & 0xF];
            break;

        case 0x7: // 8XY5 Vx = Vy - Vx; Vf = 1 if Vy > Vx, 0 otherwise
            // Set the carry flag
            reg.V[0xF] = (reg.V[(ins >> 8) & 0xF] < reg.V[(ins >> 4) & 0xF]);
            reg.V[(ins >> 8) & 0xF] =
                reg.V[(ins >> 4) & 0xF] - reg.V[(ins >> 8) & 0xF];
            break;

        case 0x6: // 8XY6 If the least-significant bit of Vx is 1, then VF is
                  // set to 1, otherwise 0. Then Vx is divided by 2
            // Set the carry flag
            reg.V[0xF] = (reg.V[(ins >> 8) & 0xF] & 1);
            reg.V[(ins >> 8) & 0xF] >>= 1;
            break;

        case 0xE: // 8XYE If the most-significant bit of Vx is 1, then VF is
                  // set to 1, otherwise to 0. Then Vx is multiplied by 2.
            // Set the carry flag
            reg.V[0xF] = ((reg.V[(ins >> 8) & 0xF] >> 7) & 1);
            reg.V[(ins >> 8) & 0xF] <<= 1;
            break;
        }
        break;

    case 0xA: // ANNN set register I to NNN
        reg.I = (ins & 0xFFF);
        break;

    case 0xB: // BNNN jump to NNN + V0
        reg.PC = (ins & 0xFFF) + reg.V[0x0];
        break;

    case 0xC: // CXNN Vx = randNumb & NN
        reg.V[(ins >> 8) & 0xF] = c8::utils::getRandomNumber() & (ins & 0xFF);
        break;

    case 0xD: // DXYN
        drawSprite(reg.V[(ins >> 8) & 0xF], reg.V[(ins >> 4) & 0xF], ins & 0xF);
        break;

    case 0xE: // EX9E & EXA1
        if ((ins & 0xFF)
            == 0x9E) { // skip if key corresponding to Vx is pressed

            reg.PC += 2
                * c8::keyboard::keyDown(getKeyInput(), reg.V[(ins >> 8) & 0xF]);
            ;

        } else { // skip if key corresponding to Vx is not pressed

            reg.PC +=
                2 * c8::keyboard::keyUp(getKeyInput(), reg.V[(ins >> 8) & 0xF]);
        }
        break;
    case 0xF:
        switch (ins & 0xFF) {
        case 0x07: // FX07 Vx = DT
            reg.V[(ins >> 8) & 0xF] = reg.DT;
            break;

        case 0x15: // FX15 DT = Vx
            reg.DT = reg.V[(ins >> 8) & 0xF];
            break;

        case 0x18: // FX18 ST = Vx
            reg.ST = reg.V[(ins >> 8) & 0xF];
            break;

        case 0x1E: // FX1E I += Vx
            reg.I += reg.V[(ins >> 8) & 0xF];
            break;

        case 0x29: // FX29 point I to the address of the hexadecimal character
                   // in Vx
            reg.I = fontStart + 5 * (reg.V[(ins >> 8) & 0xF] & 0xF);
            break;

        case 0x0A:
            { // FX0A Vx = key pressed
                using namespace c8::keyboard;
                reg.PC -= 2 * !(keyValid(getKeyInput())); // repeat instruction
                                                          // until key is valid
                break;
            }

        case 0x33: // FX33 store the digits of decimal representation of Vx in
                   // the 3 consecutive bytes starting from address pointed by
                   // register I
            ram[reg.I]     = (reg.V[(ins >> 8) & 0xF]) / 100;
            ram[reg.I + 1] = ((reg.V[(ins >> 8) & 0xF]) / 10) % 10;
            ram[reg.I + 2] = (reg.V[(ins >> 8) & 0xF]) % 10;
            break;

        case 0x55: // FX55 copy values V0...Vx in the ram at locations I...I+x
            for (uint8_t index = 0x0;
                 index <= static_cast<uint8_t>((ins >> 8) & 0xF); ++index) {
                ram[reg.I + index] = reg.V[index];
            }
            break;

        case 0x65: // FX65 read value V0...Vx from the ram at location I...I+x
            for (uint8_t index = 0x0;
                 index <= static_cast<uint8_t>((ins >> 8) & 0xF); ++index) {
                reg.V[index] = ram[reg.I + index];
            }
            break;
        }
        break;
    }
}
} // namespace c8
