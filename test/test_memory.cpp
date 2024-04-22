#define BOOST_TEST_MODULE memory_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "memory.h"
#include "types.h"

#include <random>
#include <sstream>
#include <string>

static unsigned int seed = 5489;
static size_t const trials = 1000;

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
