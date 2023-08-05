#include "keyboard.hpp"

namespace c8::keyboard{

uint8_t convertScanCodeToKey(SDL_Scancode code){
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

SDL_Scancode convertKeyToScanCode(uint8_t key){
    switch (key) {
    case 0x0:
        return SDL_SCANCODE_X;
    case 0x1:
        return SDL_SCANCODE_1;
    case 0x2:
        return SDL_SCANCODE_2;
    case 0x3:
        return SDL_SCANCODE_3;
    case 0x4:
        return SDL_SCANCODE_Q;
    case 0x5:
        return SDL_SCANCODE_W;
    case 0x6:
        return SDL_SCANCODE_E;
    case 0x7:
        return SDL_SCANCODE_A;
    case 0x8:
        return SDL_SCANCODE_S;
    case 0x9:
        return SDL_SCANCODE_D;
    case 0xA:
        return SDL_SCANCODE_Z;
    case 0xB:
        return SDL_SCANCODE_C;
    case 0xC:
        return SDL_SCANCODE_4;
    case 0xD:
        return SDL_SCANCODE_R;
    case 0xE:
        return SDL_SCANCODE_F;
    case 0xF:
        return SDL_SCANCODE_V;
    }
}

SDL_Scancode getKeyPressed(){
    //bool quit = false;
    SDL_Event event;
    SDL_Scancode keyPressed = SDL_SCANCODE_OUT;
    if(SDL_WaitEvent(&event) == 1){
        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            keyPressed = event.key.keysym.scancode;
    }
    //SDL_FlushEvent(SDL_KEYDOWN);
    //SDL_FlushEvent(SDL_KEYUP);
    return keyPressed;
    /*
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
            break;
        }
        else if (event.type == SDL_KEYDOWN) {
            return event.key.keysym.scancode;
        }
        SDL_Delay(2);
        //using namespace std::chrono_literals;
        //std::this_thread::sleep_for( 1ms );
    }

    return SDL_SCANCODE_OUT;*/
}

}