#define BOOST_TEST_MODULE iset_block2_tests
#include <boost/test/included/unit_test.hpp>
#include <climits>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#include "bits.h"
#include "instruction_set.h"
#include "memory.h"
#include "types.h"

constexpr static int BYTE_MAX =
    static_cast<int>(std::numeric_limits<Byte>::max());
constexpr static Byte HALF_MAX = 0xF;
constexpr static Byte BLOCK_CODE = 0b1000'0000;
constexpr static Byte OP_SHIFT = 0x3;
constexpr static Byte REG_MASK = 0b0111;

BOOST_AUTO_TEST_CASE(add_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM ||
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & REG_MASK;

    // opcode for add is 0 so we exclude it here
    const Byte instruction = BLOCK_CODE | regCode;

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

  // we only test a single register here, since basic test
  // covers register selection
  const Byte regCode = 0x1;
  const Byte instruction = BLOCK_CODE | regCode;

  std::vector<std::pair<Byte, Byte>> zeroTests = {
      {0x0, 0x0},  {0xFF, 0x1}, {0xFE, 0x2}, {0xFD, 0x3},
      {0xFC, 0x4}, {0xFB, 0x5}, {0xCD, 0x33}};

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

  // we only test a single register here, since basic test
  // covers register selection
  const Byte regCode = 0x1;
  const Byte instruction = BLOCK_CODE | regCode;

  for (Byte a = 0; a <= HALF_MAX; a++) {
    for (Byte b = 0; b <= HALF_MAX; b++) {
      // clear half carry
      mem.clear_half_carry_flag();

      // set registers
      mem.set_r8(Memory::ByteRegisters::A, a);
      mem.set_r8(regCode, b);

      // add
      iset.parse_and_execute(instruction);

      // check half carry
      if ((a + b) > HALF_MAX) {
        BOOST_TEST(mem.get_half_carry_flag());
      } else {
        BOOST_TEST(!mem.get_half_carry_flag());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(adc_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x1;

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM ||
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & REG_MASK;
    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

    for (int carry = 0; carry <= 1; carry++) {
      for (int a = 0; a <= BYTE_MAX; a++) {
        for (int b = 0; b <= BYTE_MAX; b++) {
          // clear test flags
          mem.clear_sub_flag();
          mem.clear_carry_flag();

          // set registers
          mem.set_r8(Memory::ByteRegisters::A, a);
          mem.set_r8(regCode, b);

          // set carry flag
          if (carry == 1) {
            mem.set_carry_flag();
          } else {
            mem.clear_carry_flag();
          }

          // add
          iset.parse_and_execute(instruction);

          // check sum
          BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) ==
                     static_cast<Byte>(a + b + carry));

          // check N flag
          BOOST_TEST(!mem.get_sub_flag());

          // check carry flag
          if ((a + b + carry) > BYTE_MAX) {
            BOOST_TEST(mem.get_carry_flag());
          } else {
            BOOST_TEST(!mem.get_carry_flag());
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(adc_a_r8_zero_flag) {
  Memory mem;
  InstructionSet iset(mem);

  // only testing a single register since basic test covers register select
  const Byte opCode = 0x1;
  const Byte regCode = 0x1;

  const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

  std::vector<std::tuple<Byte, Byte, Byte>> zeroTests = {
      std::make_tuple(0x0, 0x0, 0x0),  std::make_tuple(0xFF, 0x1, 0x0),
      std::make_tuple(0xFF, 0x0, 0x1), std::make_tuple(0xFE, 0x2, 0x0),
      std::make_tuple(0xFE, 0x1, 0x1), std::make_tuple(0xFD, 0x3, 0x0),
      std::make_tuple(0xFD, 0x2, 0x1), std::make_tuple(0xFC, 0x4, 0x0),
      std::make_tuple(0xFC, 0x3, 0x1), std::make_tuple(0xFB, 0x5, 0x0),
      std::make_tuple(0xFB, 0x4, 0x1), std::make_tuple(0xCD, 0x33, 0x0),
      std::make_tuple(0xCD, 0x32, 0x1)};

  for (auto const& [a, b, carry] : zeroTests) {
    // clear test flags
    mem.clear_zero_flag();

    // set registers
    mem.set_r8(Memory::ByteRegisters::A, a);
    mem.set_r8(regCode, b);

    if (carry == 1) {
      mem.set_carry_flag();
    } else {
      mem.clear_carry_flag();
    }

    // add
    iset.parse_and_execute(instruction);

    // check zero flag
    BOOST_TEST(mem.get_zero_flag());
  }
}

BOOST_AUTO_TEST_CASE(adc_a_r8_half_carry) {
  Memory mem;
  InstructionSet iset(mem);

  // only testing a single register since basic test covers register select
  const Byte opCode = 0x1;
  const Byte regCode = 0x1;

  const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

  for (int carry = 0; carry <= 1; carry++) {
    for (Byte a = 0; a <= HALF_MAX; a++) {
      for (Byte b = 0; b <= HALF_MAX; b++) {
        // clear half carry
        mem.clear_half_carry_flag();

        if (carry == 1) {
          mem.set_carry_flag();
        } else {
          mem.clear_carry_flag();
        }

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(regCode, b);

        // add
        iset.parse_and_execute(instruction);

        // check half carry
        if ((a + b + carry) > HALF_MAX) {
          BOOST_TEST(mem.get_half_carry_flag());
        } else {
          BOOST_TEST(!mem.get_half_carry_flag());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(sub_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x2;

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM ||
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & REG_MASK;
    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

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

        // check diff
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) ==
                   static_cast<Byte>(a - b));

        // check N flag
        BOOST_TEST(mem.get_sub_flag());

        // check carry/borrow flag
        if (b > a) {
          BOOST_TEST(mem.get_carry_flag());
        } else {
          BOOST_TEST(!mem.get_carry_flag());
        }

        // check zero flag
        if (a == b) {
          BOOST_TEST(mem.get_zero_flag());
        } else {
          BOOST_TEST(!mem.get_zero_flag());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(sub_a_r8_half_borrow) {
  Memory mem;
  InstructionSet iset(mem);

  // only testing a single register since basic test covers register select
  const Byte opCode = 0x2;
  const Byte regCode = 0x1;

  const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

  for (Byte a = 0; a <= HALF_MAX; a++) {
    for (Byte b = 0; b <= HALF_MAX; b++) {
      // clear half carry
      mem.clear_half_carry_flag();

      // set registers
      mem.set_r8(Memory::ByteRegisters::A, a);
      mem.set_r8(regCode, b);

      // subtract
      iset.parse_and_execute(instruction);

      // check half carry/borrow
      if (b > a) {
        BOOST_TEST(mem.get_half_carry_flag());
      } else {
        BOOST_TEST(!mem.get_half_carry_flag());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(sbc_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x3;

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM ||
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & REG_MASK;
    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

    for (int carry = 0; carry <= 1; carry++) {
      for (int a = 0; a <= BYTE_MAX; a++) {
        for (int b = 0; b <= BYTE_MAX; b++) {
          // clear test flags
          mem.clear_sub_flag();

          if (carry == 1) {
            mem.set_carry_flag();
          } else {
            mem.clear_carry_flag();
          }

          // set registers
          mem.set_r8(Memory::ByteRegisters::A, a);
          mem.set_r8(regCode, b);

          // add
          iset.parse_and_execute(instruction);

          // check diff
          BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) ==
                     static_cast<Byte>(a - (b + carry)));

          // check N flag
          BOOST_TEST(mem.get_sub_flag());

          // check carry/borrow flag
          if ((b + carry) > a) {
            BOOST_TEST(mem.get_carry_flag());
          } else {
            BOOST_TEST(!mem.get_carry_flag());
          }

          // check zero flag
          if (static_cast<Byte>(a) == static_cast<Byte>(b + carry)) {
            BOOST_TEST(mem.get_zero_flag());
          } else {
            BOOST_TEST(!mem.get_zero_flag());
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(sbc_a_r8_half_borrow) {
  Memory mem;
  InstructionSet iset(mem);

  // only testing a single register since basic test covers register select
  const Byte opCode = 0x3;
  const Byte regCode = 0x1;

  const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

  for (Byte carry = 0; carry <= 1; carry++) {
    for (Byte a = 0; a <= HALF_MAX; a++) {
      for (Byte b = 0; b <= HALF_MAX; b++) {
        // clear half carry
        mem.clear_half_carry_flag();

        if (carry == 1) {
          mem.set_carry_flag();
        } else {
          mem.clear_carry_flag();
        }

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(regCode, b);

        // subtract
        iset.parse_and_execute(instruction);

        // check half carry/borrow
        if ((b + carry) > a) {
          BOOST_TEST(mem.get_half_carry_flag());
        } else {
          BOOST_TEST(!mem.get_half_carry_flag());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(and_a_r8) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x4;

  for (Byte reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::A ||
        reg == Memory::ByteRegisters::HL_MEM) {
      continue;
    }

    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | reg;

    for (int a = 0; a <= BYTE_MAX; a++) {
      for (int b = 0; b <= BYTE_MAX; b++) {
        // clear flags
        mem.clear_zero_flag();
        mem.clear_sub_flag();
        mem.clear_half_carry_flag();
        mem.clear_carry_flag();

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(reg, b);

        // execute
        iset.parse_and_execute(instruction);

        const Byte expectVal = a & b;

        // checks
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) == expectVal);

        if (expectVal == 0) {
          BOOST_TEST(mem.get_zero_flag());
        } else {
          BOOST_TEST(!mem.get_zero_flag());
        }

        BOOST_TEST(!mem.get_sub_flag());
        BOOST_TEST(mem.get_half_carry_flag());
        BOOST_TEST(!mem.get_carry_flag());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(xor_a_r8) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x5;

  for (Byte reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::A ||
        reg == Memory::ByteRegisters::HL_MEM) {
      continue;
    }

    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | reg;

    for (int a = 0; a <= BYTE_MAX; a++) {
      for (int b = 0; b <= BYTE_MAX; b++) {
        // clear flags
        mem.clear_zero_flag();
        mem.clear_sub_flag();
        mem.clear_half_carry_flag();
        mem.clear_carry_flag();

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(reg, b);

        // execute
        iset.parse_and_execute(instruction);

        const Byte expectVal = a ^ b;

        // checks
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) == expectVal);

        if (expectVal == 0) {
          BOOST_TEST(mem.get_zero_flag());
        } else {
          BOOST_TEST(!mem.get_zero_flag());
        }

        BOOST_TEST(!mem.get_sub_flag());
        BOOST_TEST(!mem.get_half_carry_flag());
        BOOST_TEST(!mem.get_carry_flag());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(or_a_r8) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x6;

  for (Byte reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::A ||
        reg == Memory::ByteRegisters::HL_MEM) {
      continue;
    }

    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | reg;

    for (int a = 0; a <= BYTE_MAX; a++) {
      for (int b = 0; b <= BYTE_MAX; b++) {
        // clear flags
        mem.clear_zero_flag();
        mem.clear_sub_flag();
        mem.clear_half_carry_flag();
        mem.clear_carry_flag();

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(reg, b);

        // execute
        iset.parse_and_execute(instruction);

        const Byte expectVal = a | b;

        // checks
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) == expectVal);

        if (expectVal == 0) {
          BOOST_TEST(mem.get_zero_flag());
        } else {
          BOOST_TEST(!mem.get_zero_flag());
        }

        BOOST_TEST(!mem.get_sub_flag());
        BOOST_TEST(!mem.get_half_carry_flag());
        BOOST_TEST(!mem.get_carry_flag());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(cp_a_r8_basic) {
  Memory mem;
  InstructionSet iset(mem);

  const Byte opCode = 0x7;

  for (int reg = 0; reg < CHAR_BIT; reg++) {
    if (reg == Memory::ByteRegisters::HL_MEM ||
        reg == Memory::ByteRegisters::A) {
      continue;
    }

    const Byte regCode = reg & REG_MASK;
    const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

    for (int a = 0; a <= BYTE_MAX; a++) {
      for (int b = 0; b <= BYTE_MAX; b++) {
        // clear test flags
        mem.clear_sub_flag();
        mem.clear_carry_flag();

        // set registers
        mem.set_r8(Memory::ByteRegisters::A, a);
        mem.set_r8(regCode, b);

        // compare
        iset.parse_and_execute(instruction);

        // confirm register A is unchanged
        BOOST_TEST(mem.get_r8(Memory::ByteRegisters::A) == a);

        // check N flag
        BOOST_TEST(mem.get_sub_flag());

        // check carry/borrow flag
        if (b > a) {
          BOOST_TEST(mem.get_carry_flag());
        } else {
          BOOST_TEST(!mem.get_carry_flag());
        }

        // check zero flag
        if (a == b) {
          BOOST_TEST(mem.get_zero_flag());
        } else {
          BOOST_TEST(!mem.get_zero_flag());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(cp_a_r8_half_borrow) {
  Memory mem;
  InstructionSet iset(mem);

  // only testing a single register since basic test covers register select
  const Byte opCode = 0x7;
  const Byte regCode = 0x1;

  const Byte instruction = BLOCK_CODE | (opCode << OP_SHIFT) | regCode;

  for (Byte a = 0; a <= HALF_MAX; a++) {
    for (Byte b = 0; b <= HALF_MAX; b++) {
      // clear half carry
      mem.clear_half_carry_flag();

      // set registers
      mem.set_r8(Memory::ByteRegisters::A, a);
      mem.set_r8(regCode, b);

      // compare
      iset.parse_and_execute(instruction);

      // check half carry/borrow
      if (b > a) {
        BOOST_TEST(mem.get_half_carry_flag());
      } else {
        BOOST_TEST(!mem.get_half_carry_flag());
      }
    }
  }
}
