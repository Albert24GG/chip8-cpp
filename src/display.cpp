#include "display.hpp"
#include <SDL2/SDL_render.h>

namespace c8::display {

inline void Window::clearScreen() {
  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);
}

void Window::renderScreen() {
  clearScreen();

  SDL_Rect pixel;
  pixel.w = pixel.h = pixelSize;

  for (uint16_t row = 0; row < 32; ++row) {
    for (uint16_t col = 0; col < 64; ++col) {
      pixel.x = col * pixelSize;
      pixel.y = row * pixelSize;

      bool pixelOn = this->screen[row][col];

      SDL_SetRenderDrawColor(this->renderer, 255 * pixelOn, 255 * pixelOn,
                             255 * pixelOn, 255);

      SDL_RenderFillRect(this->renderer, &pixel);
    }
  }

  SDL_RenderPresent(
      this->renderer); // update the window with the rendered screen
}

} // namespace c8::display
