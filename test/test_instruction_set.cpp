#define BOOST_TEST_MODULE instruction_set_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "instruction_set.h"
#include "memory.h"
#include "types.h"

#include <climits>
#include <limits>
#include <utility>
#include <vector>

constexpr static int BYTE_MAX = static_cast<int>(std::numeric_limits<Byte>::max());

BOOST_AUTO_TEST_CASE(load_r8_r8) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte destShift = 3;
  const Byte instructionBase = 0b0100'0000;

  const Word addr = 0xBEEF;
  const Byte lo = KeaBits::getLowByte(addr);
  const Byte hi = KeaBits::getHighByte(addr);

  for (int src = 0; src < CHAR_BIT; src++) {
    for (int dest = 0; dest < CHAR_BIT; dest++) {
      if (src == dest) {
        continue;
      }

      if (src == Memory::ByteRegisters::HL_MEM) {
        mem.set_r8(Memory::ByteRegisters::L, lo);
        mem.set_r8(Memory::ByteRegisters::H, hi);
      }

      const Byte instruction = instructionBase | (dest << destShift) | src;

      for (int val = 0; val <= BYTE_MAX; val++) {
        mem.set_r8(src, val);
        iset.parse_and_execute(instruction);
        BOOST_TEST(mem.get_r8(dest) == val);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(add_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte blockCode = 0b1000'0000;
  const Byte regMask = 0b0111;

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM || 
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & regMask;

    // opcode for add is 0 so we exclude it here
    const Byte instruction = blockCode | regCode;

    for (int a = 0; a <= BYTE_MAX; a++) {
      for (int b = 0; b <= BYTE_MAX; b++) {
        // clear test flags
        mem.clear_sub_flag();
        mem.clear_carry_flag();

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(regCode, b);

        // add
        iset.parse_and_execute(instruction);

        // check sum
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) == 
            static_cast<Byte>(a + b));

        // check N flag
        BOOST_TEST(!mem.get_sub_flag());

        // check carry flag
        if ((a + b) > BYTE_MAX) {
          BOOST_TEST(mem.get_carry_flag());
        } else {
          BOOST_TEST(!mem.get_carry_flag());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(add_a_r8_zero_flag) {
  Memory mem;
  InstructionSet iset(mem);

  // this should be the code for ADD A, B which is fine
  const Byte instruction = 0b1000'0000;
  const Byte regCode = 0x0;

  std::vector<std::pair<Byte, Byte>> zeroTests = {
    {0x0, 0x0}, {0xFF, 0x1}, {0xFE, 0x2}, {0xFD, 0x3},
    {0xFC, 0x4}, {0xFB, 0x5}, {0xCD, 0x33}
  };

  for (auto const& [a, b] : zeroTests) {
    // clear test flags
    mem.clear_zero_flag();

    // set registers
    mem.set_r8(Memory::ByteRegisters::A, a);
    mem.set_r8(regCode, b);

    // add
    iset.parse_and_execute(instruction);

    // check zero flag
    BOOST_TEST(mem.get_zero_flag());
  }
}

BOOST_AUTO_TEST_CASE(add_a_r8_half_carry) {
  Memory mem;
  InstructionSet iset(mem);

  // this should be the code for ADD A, B which is fine
  const Byte instruction = 0b1000'0000;
  const Byte regCode = 0x0;
  const Byte halfMax = 0xF;

  for (Byte a = 0; a <= halfMax; a++) {
    for (Byte b = 0; b <= halfMax; b++) {
      // clear half carry
      mem.clear_half_carry_flag();

      // set registers
      mem.set_r8(Memory::ByteRegisters::A, a);
      mem.set_r8(regCode, b);

      // add
      iset.parse_and_execute(instruction);

      // check half carry
      if ((a + b) > halfMax) {
        BOOST_TEST(mem.get_half_carry_flag());
      } else {
        BOOST_TEST(!mem.get_half_carry_flag());
      }
    }
  }
}