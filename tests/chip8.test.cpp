#include "../src/chip8.h"
#include <ctime>
#include <gtest/gtest.h>
#include <iostream>
#include <stack>

// test fixture
class Chip8_test : public ::testing::Test {
protected:
  Chip8_test() : shouldRedraw(false), pendingInput(false) {}
  void SetUp(const BYTE (&data)[2]) {
    cpu.Memory[0x200] = data[0];
    cpu.Memory[0x201] = data[1];
  }
  void fde_cycle() {
    cpu.fde_cycle(shouldRedraw, pendingInput);
  }

  Chip8 cpu;
  bool shouldRedraw;
  bool pendingInput;
};

// tests
TEST_F(Chip8_test, I_00e0) {
  SetUp({0x00, 0xe0});
  fde_cycle();
  EXPECT_TRUE(shouldRedraw);
}

TEST_F(Chip8_test, I_00ee) {
  SetUp({0x00, 0xee});
  cpu.Stack.push(0xbeef);
  cpu.SP = 0x01;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0xbeef);
  EXPECT_EQ(cpu.SP, 0);
}

TEST_F(Chip8_test, I_1nnn) {
  SetUp({0x1b, 0xee});
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0xbee);
}

TEST_F(Chip8_test, I_2nnn) {
  SetUp({0x2b, 0xee});
  cpu.SP = 0x01;
  fde_cycle();
  EXPECT_EQ(cpu.SP, 0x02);
  EXPECT_EQ(cpu.Stack.top(), 0x202);
  EXPECT_EQ(cpu.PC, 0xbee);
}

TEST_F(Chip8_test, I_3xkk) {
  SetUp({0x32, 0xee});
  // 1st test
  cpu.VX[2] = 0xef;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.VX[2] = 0xee;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_4xkk) {
  SetUp({0x42, 0xee});
  // 1st test
  cpu.VX[2] = 0xee;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.VX[2] = 0xef;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_5xy0) {
  SetUp({0x51, 0x20});
  // 1st test
  cpu.VX[1] = 0xee;
  cpu.VX[2] = 0xef;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.VX[2] = 0xee;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_6xkk) {
  SetUp({0x65, 0x43});
  fde_cycle();
  EXPECT_EQ(cpu.VX[5], 0x43);
}

TEST_F(Chip8_test, I_7xkk) {
  SetUp({0x75, 0x0d});
  cpu.VX[5] = 0x10;
  fde_cycle();
  EXPECT_EQ(cpu.VX[5], 0x1d);
}

TEST_F(Chip8_test, I_9xy0) {
  SetUp({0x91, 0x20});
  // 1st test
  cpu.VX[1] = 0xee;
  cpu.VX[2] = 0xee;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.VX[2] = 0xef;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_Annn) {
  SetUp({0xA5, 0x43});
  fde_cycle();
  EXPECT_EQ(cpu.I, 0x543);
}

TEST_F(Chip8_test, I_Bnnn) {
  SetUp({0xB5, 0xe0});
  cpu.VX[0] = 0x0e;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x5ee);
}

/* TO_DO
TEST_F(Chip8_test, I_Cxkk) {
  SetUp({0xC3, 0xee});
  fde_cycle();
  EXPECT_EQ(shouldRedraw, true);
}
*/

TEST_F(Chip8_test, I_Dxyn) {
  SetUp({0xD2, 0x35});
  cpu.I = 0x50;
  cpu.VX[2] = 61;
  cpu.VX[3] = 29;
  fde_cycle();
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 64; ++j) {
      std::cout << unsigned(cpu.display[64 * i + j]);
    }
    std::cout << '\n';
  }
  EXPECT_EQ(shouldRedraw, true);
}

TEST_F(Chip8_test, I_Ex9e) {
  SetUp({0xE1, 0x9e});
  // 1st test
  cpu.VX[1] = 2;
  cpu.keyboard[2] = 0;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.keyboard[2] = 1;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_ExA1) {
  SetUp({0xE1, 0xA1});
  // 1st test
  cpu.VX[1] = 2;
  cpu.keyboard[2] = 1;
  fde_cycle();
  EXPECT_NE(cpu.PC, 0x204);

  // 2nd test
  cpu.PC = 0x200;
  cpu.keyboard[2] = 0;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x204);
}

TEST_F(Chip8_test, I_Fx07) {
  SetUp({0xF8, 0x07});
  cpu.DT = 0xab;
  fde_cycle();
  EXPECT_EQ(cpu.VX[8], 0xab);
}

TEST_F(Chip8_test, I_Fx0A) {
  SetUp({0xF5, 0x0A});
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x200);
  EXPECT_TRUE(pendingInput);
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x200);
  cpu.keyboard[0xa] = 1;
  fde_cycle();
  EXPECT_EQ(cpu.PC, 0x202);
  EXPECT_EQ(cpu.VX[5], 0xa);
}

TEST_F(Chip8_test, I_Fx15) {
  SetUp({0xF8, 0x15});
  cpu.VX[8] = 0xab;
  fde_cycle();
  EXPECT_EQ(cpu.DT, 0xab);
}

TEST_F(Chip8_test, I_Fx18) {
  SetUp({0xF8, 0x18});
  cpu.VX[8] = 0xab;
  fde_cycle();
  EXPECT_EQ(cpu.ST, 0xab);
}

TEST_F(Chip8_test, I_Fx1E) {
  SetUp({0xF5, 0x1E});
  cpu.VX[5] = 0xa0;
  cpu.I = 0x0b;
  fde_cycle();
  EXPECT_EQ(cpu.I, 0xab);
}

TEST_F(Chip8_test, I_Fx29) {
  SetUp({0xF4, 0x29});
  cpu.VX[4] = 0x0a;
  fde_cycle();
  EXPECT_EQ(cpu.I, 0x50 + 10 * 5);
}

TEST_F(Chip8_test, I_Fx33) {
  SetUp({0xF2, 0x33});
  cpu.I = 0x250;
  cpu.VX[2] = 0xff;
  fde_cycle();
  EXPECT_EQ(cpu.Memory[cpu.I], 2);
  EXPECT_EQ(cpu.Memory[cpu.I + 1], 5);
  EXPECT_EQ(cpu.Memory[cpu.I + 2], 5);
}

TEST_F(Chip8_test, I_Fx55) {
  SetUp({0xF3, 0x55});
  cpu.I = 0x250;
  cpu.VX[0] = 0x10;
  cpu.VX[1] = 0x20;
  cpu.VX[2] = 0x30;
  cpu.VX[3] = 0x40;
  fde_cycle();
  EXPECT_EQ(cpu.Memory[cpu.I], 0x10);
  EXPECT_EQ(cpu.Memory[cpu.I + 1], 0x20);
  EXPECT_EQ(cpu.Memory[cpu.I + 2], 0x30);
  EXPECT_EQ(cpu.Memory[cpu.I + 3], 0x40);
}

TEST_F(Chip8_test, I_Fx65) {
  SetUp({0xF3, 0x55});
  cpu.I = 0x250;
  cpu.Memory[cpu.I] = 0x10;
  cpu.Memory[cpu.I + 1] = 0x20;
  cpu.Memory[cpu.I + 2] = 0x30;
  cpu.Memory[cpu.I + 3] = 0x40;
  fde_cycle();
  EXPECT_EQ(cpu.VX[0], 0x10);
  EXPECT_EQ(cpu.VX[1], 0x20);
  EXPECT_EQ(cpu.VX[2], 0x30);
  EXPECT_EQ(cpu.VX[3], 0x40);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
