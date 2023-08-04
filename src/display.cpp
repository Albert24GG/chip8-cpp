#include "display.hpp"

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

          bool pixelOn = (this->screen[row] & (1LL << (63 - col))) != 0;

          SDL_SetRenderDrawColor(this->renderer, 255 * pixelOn, 255 * pixelOn,
                                 255 * pixelOn, 255);

          SDL_RenderFillRect(this->renderer, &pixel);
      }
  }

  SDL_RenderPresent(
      this->renderer); // update the window with the rendered screen
}

void Window::resetScreen() {
  this->screen.fill(0);
}

bool Window::applySprite(const uint8_t row, const uint64_t sprite){
  uint64_t newSprite {this->screen[row] ^ sprite};
  this->screen[row] = newSprite;

  // check if any pixels were erased
  return ((this->screen[row] & newSprite) != this->screen[row]);
}

/**
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

void Window::resetScreen() {
  for(auto &row : this->screen){
    for(auto &cell : row)
      cell = false;
  }
}

void Window::togglePixel(const uint8_t xCoord, const uint8_t yCoord) {
  this->screen[yCoord][xCoord] ^= 1;
}
**/

} // namespace c8::display
