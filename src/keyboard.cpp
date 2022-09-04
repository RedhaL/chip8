#include "keyboard.h"

// key mapping
std::map<SDL_Scancode, int> keyMap = {
    {SDL_SCANCODE_1, 1},  {SDL_SCANCODE_2, 2},  {SDL_SCANCODE_3, 3},
    {SDL_SCANCODE_Q, 4},  {SDL_SCANCODE_W, 5},  {SDL_SCANCODE_E, 6},
    {SDL_SCANCODE_A, 7},  {SDL_SCANCODE_S, 8},  {SDL_SCANCODE_D, 9},
    {SDL_SCANCODE_Z, 10}, {SDL_SCANCODE_X, 0},  {SDL_SCANCODE_C, 11},
    {SDL_SCANCODE_4, 12}, {SDL_SCANCODE_R, 13}, {SDL_SCANCODE_F, 14},
    {SDL_SCANCODE_V, 15},
};

void Keyboard::handleInput(uint8_t* keyboard, bool& quit, bool& pendingInput) {
  // handle input
    SDL_Event e;
    while (SDL_PollEvent(&e) || pendingInput) {
      switch (e.type) {
        case SDL_QUIT: {
          quit = true;
          pendingInput = false;
          break;
        }
        case SDL_KEYDOWN: {
          keyboard[keyMap[e.key.keysym.scancode]] = 1;
          pendingInput = false;
          break;
        }
        case SDL_KEYUP: {
          keyboard[keyMap[e.key.keysym.scancode]] = 0;
          pendingInput = false;
          break;
        }
        default:
          break;
      }
    }
}

