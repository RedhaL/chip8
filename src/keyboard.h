#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL2/SDL.h>
#include <iostream>
#include <map>

class Keyboard {
public:
  void handleInput(uint8_t *, bool &, bool &);
};

#endif