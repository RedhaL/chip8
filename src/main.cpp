#include "chip8.h"
#include "display.h"
#include "keyboard.h"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

void decrementTimers(Chip8 &cpu, bool &quit) {
  using namespace std::literals::chrono_literals;
  while (!quit) {
    cpu.decrement_timers();
    // timer's rate is 60Hz => 1000/60 ~ 16
    std::this_thread::sleep_for(16ms);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: chip8 romfile" << std::endl;
    return 1;
  }

  Chip8 cpu;
  Keyboard keyboard;
  Display display;

  // load rom
  cpu.load(argv[1]);

  // seed for random generator
  std::srand(std::time(nullptr));

  // start timers thread
  bool quit = false;
  std::thread timers(decrementTimers, std::ref(cpu), std::ref(quit));

  // start emulation loop
  const int frameDelay = 1;
  int frameStart;
  int frameTime;
  bool pendingInput = false;

  while (!quit) {
    frameStart = SDL_GetTicks();

    // emulate cycle
    bool shouldRender = false;
    cpu.fde_cycle(shouldRender, pendingInput);

    keyboard.handleInput(cpu.keyboard, quit, pendingInput);

    if (shouldRender) {
      display.draw(cpu.getDisplay());
    }

    frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < frameDelay) {
      SDL_Delay(frameDelay - frameTime);
    }
  }

  // wait for threads
  timers.join();
}
