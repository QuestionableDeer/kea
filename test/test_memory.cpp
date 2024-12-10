#define BOOST_TEST_MODULE memory_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "memory.h"
#include "types.h"

#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static unsigned int seed = 5489;
static size_t const trials = 1000;

constexpr static int BYTE_MAX = static_cast<int>(std::numeric_limits<Byte>::max());
constexpr static int WORD_MAX = static_cast<int>(std::numeric_limits<Word>::max());

BOOST_AUTO_TEST_CASE(fetch_set_byte_property) {
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<Byte> entryDist;
  std::uniform_int_distribution<Word> addrDist;

  Memory memory;

  for (size_t i = 0; i < trials; i++) {
    Byte val = entryDist(eng);
    Word addr = addrDist(eng);

    memory.setByte(addr, val);
    BOOST_TEST(memory.fetchByte(addr) == val);
  }
}

BOOST_AUTO_TEST_CASE(fetch_set_word_property) {
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<Word> dist;

  Memory memory;

  for (size_t i = 0; i < trials; i++) {
    Word val = dist(eng);
    Word addr = dist(eng);

    memory.setWord(addr, val);
    BOOST_TEST(memory.fetchWord(addr) == val);
  }
}

BOOST_AUTO_TEST_CASE(dump_load_rom_property) {
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<Byte> dist;

  Memory mem1;

  for (size_t i = 0; i < Memory::memSize; i++) {
    mem1.setByte(i, dist(eng));
  }

  std::stringstream ss;
  mem1.dumpRom(ss);

  Memory mem2;
  mem2.loadRom(ss);

  for (size_t i = 0; i < Memory::memSize; i++) {
    BOOST_TEST(mem1.fetchByte(i) == mem2.fetchByte(i));
  }
}

BOOST_AUTO_TEST_CASE(r8_access_by_id) {
  Memory mem;

  // no ID 6 because this is memory access, [HL]
  const std::vector<Byte> validIds = {0, 1, 2, 3, 4, 5, 7};

  for (auto const& id : validIds) {
    for (int value = 0; value <= BYTE_MAX; value++) {
      mem.set_r8(id, value);
      BOOST_TEST(mem.get_r8(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_hl_mem_access_by_id) {
  Memory mem;
  const Word addr = 0xBEEF;
  const Byte lo = KeaBits::getLowByte(addr);
  const Byte hi = KeaBits::getHighByte(addr);

  mem.set_r8(Memory::ByteRegisters::H, hi);
  mem.set_r8(Memory::ByteRegisters::L, lo);

  const Byte regId = 6;
  for (int bval = 0; bval <= BYTE_MAX; bval++) {
    mem.set_r8(regId, bval);
    BOOST_TEST(mem.get_r8(regId) == bval);
    BOOST_TEST(mem.fetchByte(addr) == bval);
  }
}

BOOST_AUTO_TEST_CASE(r8_access_by_enum) {
  Memory mem;

  const std::vector<Memory::ByteRegisters> validIds = {
    Memory::ByteRegisters::A,
    Memory::ByteRegisters::B,
    Memory::ByteRegisters::C,
    Memory::ByteRegisters::D,
    Memory::ByteRegisters::E,
    Memory::ByteRegisters::H,
    Memory::ByteRegisters::L
  };

  for (const auto& id : validIds) {
    for (int value = 0; value <= BYTE_MAX; value++) {
      mem.set_r8(id, value);
      BOOST_TEST(mem.get_r8(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_hl_mem_access_by_enum) {
  Memory mem;
  const Word addr = 0xBEEF;
  const Byte lo = KeaBits::getLowByte(addr);
  const Byte hi = KeaBits::getHighByte(addr);

  mem.set_r8(Memory::ByteRegisters::H, hi);
  mem.set_r8(Memory::ByteRegisters::L, lo);

  for (int bval = 0; bval <= BYTE_MAX; bval++) {
    mem.set_r8(Memory::ByteRegisters::HL_MEM, bval);
    BOOST_TEST(mem.get_r8(Memory::ByteRegisters::HL_MEM) == bval);
    BOOST_TEST(mem.fetchByte(addr) == bval);
  }
}

BOOST_AUTO_TEST_CASE(r16_access_by_id) {
  Memory mem;

  const Byte id16Size = 4;
  for (int id = 0; id < id16Size; id++) {
    for (int value = 0; value < WORD_MAX; value++) {
      mem.set_r16(id, value);
      BOOST_TEST(mem.get_r16(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_from_r16_validation_by_id) {
  Memory mem;

  const Byte lo = 0xEF;
  const Byte hi = 0xBE;
  const Word value = KeaBits::wordFromBytes(lo, hi);

  // BC
  const Byte bcId = 0x0;
  mem.set_r16(bcId, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::C) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::B) == hi);

  // DE
  const Byte deId = 0x1;
  mem.set_r16(deId, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::E) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::D) == hi);

  // HL
  const Byte hlId = 0x2;
  mem.set_r16(hlId, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::L) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::H) == hi);
}

BOOST_AUTO_TEST_CASE(sp_validation_by_id) {
  Memory mem;

  const Byte spId = 0x3;
  Word value = 0xBEEF;
  mem.set_r16(spId, value);

  BOOST_TEST(mem.sp == value);
}

BOOST_AUTO_TEST_CASE(r16_access_by_enum) {
  Memory mem;

  const std::vector<Memory::WordRegisters> validIds = {
    Memory::WordRegisters::BC,
    Memory::WordRegisters::DE,
    Memory::WordRegisters::HL,
    Memory::WordRegisters::SP
  };

  for (const auto& id : validIds) {
    for (int value = 0; value < WORD_MAX; value++) {
      mem.set_r16(id, value);
      BOOST_TEST(mem.get_r16(id) == value);
    }
  }
}

BOOST_AUTO_TEST_CASE(r8_from_r16_validation_by_enum) {
  Memory mem;

  const Byte lo = 0xEF;
  const Byte hi = 0xBE;
  const Word value = KeaBits::wordFromBytes(lo, hi);

  // BC
  mem.set_r16(Memory::WordRegisters::BC, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::C) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::B) == hi);

  // DE
  mem.set_r16(Memory::WordRegisters::DE, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::E) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::D) == hi);

  // HL
  mem.set_r16(Memory::WordRegisters::HL, value);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::L) == lo);
  BOOST_TEST(mem.get_r8(Memory::ByteRegisters::H) == hi);
}

BOOST_AUTO_TEST_CASE(sp_validation_by_enum) {
  Memory mem;

  Word value = 0xBEEF;
  mem.set_r16(Memory::WordRegisters::SP, value);

  BOOST_TEST(mem.sp == value);
}

BOOST_AUTO_TEST_CASE(invalid_r16_set) {
  Memory mem;

  // trying to access non-existent register
  Byte badId = 5;
  BOOST_REQUIRE_THROW(mem.set_r16(badId, 0xBEEF), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid_r16_get) {
  Memory mem;

  // trying to access non-existent register
  Byte badId = 5;
  Word value = 0;
  BOOST_REQUIRE_THROW((value = mem.get_r16(badId)), std::runtime_error);
}
