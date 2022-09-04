#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

class Chip8 {
private:
  uint16_t PC;
  uint16_t I;
  uint8_t SP;
  uint8_t DT;
  uint8_t ST;
  uint8_t Memory[4096] = {};
  uint8_t Display[2048] = {};
  uint8_t VX[16];
  std::stack<uint16_t> Stack;

public:
  uint8_t keyboard[16] = {};

  Chip8();

  void load(const std::string &);

  void fde_cycle(bool &, bool &);

  void decrement_timers();

  uint8_t *getDisplay() {
    return Display;
  }
};

#endif