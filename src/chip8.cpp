#include "chip8.h"

Chip8::Chip8() : PC(0x200), DT(0), ST(0) {
  // font is stored at address 0x50
  uint8_t FONT[0x50] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // Load the ROM into memory
  memcpy(Memory + 0x50, FONT, sizeof(FONT));
}

void Chip8::decrement_timers() {
  if (DT != 0) {
    DT--;
  }
  if (ST != 0) {
    ST--;
  }
}

/*
 * Load the ROM into memory
 */
void Chip8::load(const std::string &file) {
  std::ifstream rom(file, std::ios::in | std::ios::ate | std::ios::binary);
  if (!rom) {
    std::cout << "ERROR: could not open rom." << std::endl;
  }

  // read file content
  int size = rom.tellg();
  rom.seekg(0, rom.beg);
  rom.read(reinterpret_cast<char *>(Memory + 0x200), size);
  rom.close();
}

/*
 * Emulate a CPU cycle
 */

void Chip8::fde_cycle(bool &shouldRedraw, bool &pendingInput) {
  // fetch
  uint8_t b1 = Memory[PC];
  uint8_t b2 = Memory[PC + 1];
  PC += 2;

  // decode and execute
  switch (b1 >> 4) {
    case 0x0: {
      if ((b1 & 0xf) == 0) {
        if (b2 == 0xE0) {
          // 00E0 => CLS
          memset(Display, 0, sizeof(Display));
          shouldRedraw = true;
        } else if (b2 == 0xEE) {
          // 00EE => RET
          PC = Stack.top();
          Stack.pop();
          SP--;
        }
      }
      break;
    }
    case 0x1: {
      // 1nnn => JMP nnn
      PC = (((b1 & 0xf) << 8) | b2); // + 0x200;
      break;
    }
    case 0x2: {
      // 2nnn => CALL nnn
      SP++;
      Stack.push(PC);
      PC = (((b1 & 0xf) << 8) | b2); //+ 0x200
      break;
    }
    case 0x3: {
      // 3xnn => SE Vx, nn
      if ((b1 & 0xf) == 15 && b2 == 1)
        std::cout << "3F01" << std::endl;
      if (VX[b1 & 0xf] == b2)
        PC += 2;
      break;
    }
    case 0x4: {
      // 4xnn => SNE Vx, nn
      if (VX[b1 & 0xf] != b2)
        PC += 2;
      break;
    }
    case 0x5: {
      // 5xy0 => SE Vx, Vy
      if (VX[b1 & 0xf] == VX[(b2 >> 4) & 0xf])
        PC += 2;
      break;
    }
    case 0x6: {
      // 6xnn => LD Vx, nn
      VX[b1 & 0xf] = b2;
      break;
    }
    case 0x7: {
      // 7xnn => ADD Vx, nn
      VX[b1 & 0xf] = (VX[b1 & 0xf] + b2) & 0xff;
      break;
    }
    case 0x8: {
      switch (b2 & 0xf) {
        case 0x0: {
          // 8xy0 => LD Vx, Vy
          VX[b1 & 0xf] = VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x1: {
          // 8xy1 => OR Vx, Vy
          VX[b1 & 0xf] |= VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x2: {
          // 8xy2 => AND Vx, Vy
          VX[b1 & 0xf] &= VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x3: {
          // 8xy3 => XOR Vx, Vy
          VX[b1 & 0xf] ^= VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x4: {
          // 8xy4 => ADD Vx, Vy
          VX[15] = (VX[b1 & 0xf] + VX[(b2 >> 4) & 0xf]) > 0xff;
          VX[b1 & 0xf] += VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x5: {
          // 8xy5 => SUB Vx, Vy
          VX[15] = VX[b1 & 0xf] > VX[(b2 >> 4) & 0xf];
          VX[b1 & 0xf] -= VX[(b2 >> 4) & 0xf];
          break;
        }
        case 0x6: {
          // 8xy6 => SHR Vx, Vy
          VX[15] = VX[b1 & 0xf] & 0x1;
          VX[b1 & 0xf] = VX[b1 & 0xf] >> 1;
          break;
        }
        case 0x7: {
          // 8xy7 => SUBN Vx, Vy
          VX[15] = VX[(b2 >> 4) & 0xf] > VX[b1 & 0xf];
          VX[b1 & 0xf] = VX[(b2 >> 4) & 0xf] - VX[b1 & 0xf];
          break;
        }
        case 0xe: {
          // 8xyE => SHL Vx, Vy
          VX[15] = VX[b1 & 0xf] >> 7;
          VX[b1 & 0xf] = VX[b1 & 0xf] << 1;
          break;
        }
        default:
          break;
      }
      break;
    }
    case 0x9: {
      // 9xy0 => SNE Vx, Vy
      if (VX[b1 & 0xf] != VX[(b2 >> 4) & 0xf])
        PC += 2;
      break;
    }
    case 0xa: {
      // Annn => LD I, nnn
      I = ((b1 & 0xf) << 8) | b2;
      break;
    }
    case 0xb: {
      // Bnnn => JMP V0, nnn
      PC = VX[0] + (((b1 & 0xf) << 8) | b2); // + 0x200
      break;
    }
    case 0xc: {
      // Cxnn => RND Vx, nn
      VX[(b1 & 0xf)] = std::rand() & b2;
      break;
    }
    case 0xd: {
      // Dxyn => DRW Vx, Vy, n
      VX[15] = 0;
      uint8_t y = VX[(b2 >> 4) & 0xf] & 0x1f;
      int i = 0;
      while (i < (b2 & 0xf)) {
        uint8_t x = VX[b1 & 0xf] & 0x3f;

        uint8_t currentByte = Memory[I + i];

        int j = 7;
        while (j >= 0) {
          uint8_t oldValue = Display[y * 64 + x];
          Display[y * 64 + x] = ((currentByte >> j) ^ oldValue) & 0x1;

          // set VF to 1 if pixel flipped
          if (VX[15] == 0) {
            if (oldValue == 1 && Display[y * 64 + x] == 0)
              VX[15] = 1;
          }

          // check if we reached horizontal limit
          if (x < 63) {
            ++x;
            --j;
          } else {
            j = -1;
          }
        }

        // check if we reached vertical limit
        if (y < 31) {
          ++y;
          ++i;
        } else {
          i = b2 & 0xf;
        }
      }

      shouldRedraw = true;
      break;
    }
    case 0xe: {
      switch ((b2 >> 4) & 0xf) {
        case 0x9:
          // Ex9E => SKP Vx
          if (VX[b1 & 0xf] < 0x10 && keyboard[VX[b1 & 0xf]] == 1) {
            PC += 2;
          }
          break;
        case 0xa:
          // ExA1 => SKNP Vx
          if (VX[b1 & 0xf] < 0x10 && keyboard[VX[b1 & 0xf]] == 0) {
            PC += 2;
          }
          break;
      }
      break;
    }
    case 0xf: {
      switch ((b2 >> 4) & 0xf) {
        case 0x0:
          switch (b2 & 0xf) {
            case 0x7: {
              // Fx07 => LD Vx, DT
              // std::cout << "Fx07: DT = " << unsigned(DT) << std::endl;
              VX[b1 & 0xf] = DT;
              break;
            }
            case 0xa: {
              // Fx0A => LD Vx, K
              bool keyPressed = false;
              for (int i = 0; i < 16; ++i) {
                if (keyboard[i] == 1) {
                  keyPressed = true;
                  VX[b1 & 0xf] = i;
                  break;
                }
              }
              if (!keyPressed) {
                pendingInput = true;
                PC -= 2;
              }
              break;
            }
          }
          break;
        case 0x1:
          switch (b2 & 0xf) {
            case 0x5: {
              // Fx15 => LD DT, Vx
              std::cout << "Fx15: DT = " << unsigned(DT) << std::endl;
              DT = VX[b1 & 0xf] & 0xff;
              break;
            }
            case 0x8: {
              // Fx18 => LD ST, Vx
              ST = VX[b1 & 0xf] & 0xff;
              break;
            }
            case 0xe: {
              // Fx1E => ADD I, VX
              I += VX[b1 & 0xf] & 0xff;
              break;
            }
          }
          break;
        case 0x2: {
          // Fx29 => LD F, Vx
          I = 0x50 + (VX[b1 & 0xf] & 0xf) * 5;
          break;
        }
        case 0x3: {
          // Fx33 => LD B, Vx
          uint8_t vx = VX[b1 & 0xf];
          Memory[I] = vx / 100;
          vx = vx % 100;
          Memory[I + 1] = vx / 10;
          vx = vx % 10;
          Memory[I + 2] = vx;
          break;
        }
        case 0x5: {
          // Fx55 => LD [I], Vx
          memcpy(Memory + I, VX, ((b1 & 0xf) + 1));

          break;
        }
        case 0x6: {
          // Fx65 => LD Vx, [I]
          memcpy(VX, Memory + I, ((b1 & 0xf) + 1));
          break;
        }
      }
      break;
    }
    default:
      break;
  }
}
