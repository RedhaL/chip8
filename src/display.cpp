#include "display.h"

void logSDLError(std::ostream &os, const std::string &msg) {
  os << msg << " error: " << SDL_GetError() << std::endl;
}

Display::Display() {
  std::cout << "initializing the display" << std::endl;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    logSDLError(std::cout, "SDL_Init");
    return;
  }

  // create SDL window
  window =
      SDL_CreateWindow("Chip8 Emulator", 100, 100, DISPLAY_WIDTH * PIXEL_WIDTH,
                       DISPLAY_HEIGHT * PIXEL_WIDTH, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    logSDLError(std::cout, "SDL_CreateWindow");
    SDL_Quit();
    return;
  }

  // create SDL renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    logSDLError(std::cout, "SDL_CreateRenderer");
    SDL_Quit();
    return;
  }
}

Display::~Display() {
  // SDL cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Display::draw(uint8_t *display) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    if (display[i] == 1) {
      SDL_Rect whiteRect = {(i % 64) * PIXEL_WIDTH,
                            ((i - (i % 64)) / 64) * PIXEL_WIDTH, PIXEL_WIDTH,
                            PIXEL_WIDTH};
      SDL_RenderFillRect(renderer, &whiteRect);
    }
  }
  SDL_RenderPresent(renderer);
};
