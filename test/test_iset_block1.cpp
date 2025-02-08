#define BOOST_TEST_MODULE iset_block1_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "instruction_set.h"
#include "memory.h"
#include "types.h"

#include <climits>
#include <limits>

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
