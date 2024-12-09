#define BOOST_TEST_MODULE register_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "registers.h"
#include "types.h"

#include <limits>
#include <stdexcept>
#include <vector>

constexpr static int BYTE_MAX = static_cast<int>(std::numeric_limits<Byte>::max());
constexpr static int WORD_MAX = static_cast<int>(std::numeric_limits<Word>::max());

BOOST_AUTO_TEST_CASE(r8_access_by_id) {
  Registers reg;

  // no ID 6 because this is memory access, [HL]
  const std::vector<Byte> validIds = {0, 1, 2, 3, 4, 5, 7};

  for (auto const& id : validIds) {
    for (int value = 0; value <= BYTE_MAX; value++) {
      reg.set_r8(id, value);
      BOOST_TEST(reg.get_r8(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_hl_mem_set_invalid) {
  Registers reg;

  // trying to access [HL] which is memory
  Byte badId = 6;
  BOOST_REQUIRE_THROW(reg.set_r8(badId, 0xFF), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(r8_hl_mem_get_invalid) {
  Registers reg;

  // trying to access [HL] which is memory
  Byte badId = 6;
  Byte value = 0;
  BOOST_REQUIRE_THROW((value = reg.get_r8(badId)), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(r8_access_by_enum) {
  Registers reg;

  const std::vector<Registers::ByteRegisters> validIds = {
    Registers::ByteRegisters::A,
    Registers::ByteRegisters::B,
    Registers::ByteRegisters::C,
    Registers::ByteRegisters::D,
    Registers::ByteRegisters::E,
    Registers::ByteRegisters::H,
    Registers::ByteRegisters::L
  };

  for (const auto& id : validIds) {
    for (int value = 0; value <= BYTE_MAX; value++) {
      reg.set_r8(id, value);
      BOOST_TEST(reg.get_r8(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r16_access_by_id) {
  Registers reg;

  const Byte id16Size = 4;
  for (int id = 0; id < id16Size; id++) {
    for (int value = 0; value < WORD_MAX; value++) {
      reg.set_r16(id, value);
      BOOST_TEST(reg.get_r16(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_from_r16_validation_by_id) {
  Registers reg;

  const Byte lo = 0xEF;
  const Byte hi = 0xBE;
  const Word value = KeaBits::wordFromBytes(lo, hi);

  // BC
  const Byte bcId = 0x0;
  reg.set_r16(bcId, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::C) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::B) == hi);

  // DE
  const Byte deId = 0x1;
  reg.set_r16(deId, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::E) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::D) == hi);

  // HL
  const Byte hlId = 0x2;
  reg.set_r16(hlId, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::L) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::H) == hi);
}

BOOST_AUTO_TEST_CASE(sp_validation_by_id) {
  Registers reg;

  const Byte spId = 0x3;
  Word value = 0xBEEF;
  reg.set_r16(spId, value);

  BOOST_TEST(reg.sp == value);
}

BOOST_AUTO_TEST_CASE(r16_access_by_enum) {
  Registers reg;

  const std::vector<Registers::WordRegisters> validIds = {
    Registers::WordRegisters::BC,
    Registers::WordRegisters::DE,
    Registers::WordRegisters::HL,
    Registers::WordRegisters::SP
  };

  for (const auto& id : validIds) {
    for (int value = 0; value < WORD_MAX; value++) {
      reg.set_r16(id, value);
      BOOST_TEST(reg.get_r16(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_from_r16_validation_by_enum) {
  Registers reg;

  const Byte lo = 0xEF;
  const Byte hi = 0xBE;
  const Word value = KeaBits::wordFromBytes(lo, hi);

  // BC
  reg.set_r16(Registers::WordRegisters::BC, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::C) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::B) == hi);

  // DE
  reg.set_r16(Registers::WordRegisters::DE, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::E) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::D) == hi);

  // HL
  reg.set_r16(Registers::WordRegisters::HL, value);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::L) == lo);
  BOOST_TEST(reg.get_r8(Registers::ByteRegisters::H) == hi);
}

BOOST_AUTO_TEST_CASE(sp_validation_by_enum) {
  Registers reg;

  Word value = 0xBEEF;
  reg.set_r16(Registers::WordRegisters::SP, value);

  BOOST_TEST(reg.sp == value);
}

BOOST_AUTO_TEST_CASE(invalid_r16_set) {
  Registers reg;

  // trying to access [HL] which is memory
  Byte badId = 5;
  BOOST_REQUIRE_THROW(reg.set_r16(badId, 0xBEEF), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid_r16_get) {
  Registers reg;

  // trying to access [HL] which is memory
  Byte badId = 5;
  Word value = 0;
  BOOST_REQUIRE_THROW((value = reg.get_r16(badId)), std::runtime_error);
}
