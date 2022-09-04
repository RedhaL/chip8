#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <iostream>

const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;
const int PIXEL_WIDTH = 20;

class Display {
  private:
  SDL_Window *window;
  SDL_Renderer *renderer;

  public:
  Display();
  ~Display();
  void draw(uint8_t *display);
};

#endif