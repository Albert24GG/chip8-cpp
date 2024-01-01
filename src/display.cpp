#include "display.hpp"

namespace c8::display {

Window::Window(const char *title, int posX, int posY, int width, int height,
    uint32_t windowFlags, int driverIndex, uint32_t rendererFlags)
    : window{SDL_CreateWindow(title, posX, posY, width, height, windowFlags)},
      renderer{SDL_CreateRenderer(window, driverIndex, rendererFlags)} {

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_ERROR: "
                  << SDL_GetError() << '\n';
        return;
    }

    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: "
                  << SDL_GetError() << '\n';
        return;
    }
}

Window::~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

inline void Window::clearScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Window::renderScreen() {
    clearScreen();

    SDL_Rect pixel;
    pixel.w = pixel.h = pixelSize;

    for (uint16_t row = 0; row < 32; ++row) {
        for (uint16_t col = 0; col < 64; ++col) {
            pixel.x = col * pixelSize;
            pixel.y = row * pixelSize;

            bool pixelOn = (screen[row] & (1LL << (63 - col))) != 0;

            SDL_SetRenderDrawColor(renderer, 255 * pixelOn, 255 * pixelOn,
                255 * pixelOn, 255);

            SDL_RenderFillRect(renderer, &pixel);
        }
    }

    SDL_RenderPresent(renderer); // update the window with the rendered screen
}

void Window::resetScreen() { screen.fill(0); }

bool Window::applySprite(uint8_t row, uint64_t sprite) {
    uint64_t oldSprite{screen[row]};
    screen[row] ^= sprite;

    // check if any pixels were erased
    return ((screen[row] & oldSprite) != oldSprite);
}

} // namespace c8::display
