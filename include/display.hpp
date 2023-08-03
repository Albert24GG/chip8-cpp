#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>
#include <iostream>

namespace c8::display {

constexpr uint8_t pixelSize = 20;

class Window {
public:
  Window(const char *title = "Chip8 Screen", int posX = SDL_WINDOWPOS_CENTERED,
         int posY = SDL_WINDOWPOS_CENTERED, int width = 64 * pixelSize,
         int height = 32 * pixelSize, uint32_t windowFlags = SDL_WINDOW_SHOWN,
         int driverIndex = -1,
         uint32_t rendererFlags = SDL_RENDERER_ACCELERATED)
      : window{SDL_CreateWindow(title, posX, posY, width, height, windowFlags)},
        renderer{SDL_CreateRenderer(window, driverIndex, rendererFlags)} {

    if (window == nullptr) {
      std::cerr << "Window could not be created! SDL_ERROR: " << SDL_GetError()
                << '\n';
      return;
    }

    if (renderer == nullptr) {
      std::cerr << "Renderer could not be created! SDL_Error: "
                << SDL_GetError() << '\n';
      return;
    }
  }
  Window(Window &&) = default;
  Window(const Window &) = default;
  ~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void clearScreen();
  void renderScreen();
  void resetScreen();
  void togglePixel(const uint8_t xCoord, const uint8_t yCoord);
  void applySprite(const uint8_t row, const uint64_t sprite);

private:
  SDL_Window *const window;
  SDL_Renderer *const renderer;
  //std::array<std::array<bool, 64>, 32> screen{{}};
  std::array<uint64_t, 32> screen{{}};
};

} // namespace c8::display
