#define BOOST_TEST_MODULE bit_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "types.h"

#include <limits>
#include <random>
#include <utility>
#include <vector>

static unsigned int const seed = 5489;
static size_t const trials = 1000;

constexpr static int BYTE_MAX = static_cast<int>(std::numeric_limits<Byte>::max());

BOOST_AUTO_TEST_CASE(low_byte_edge_cases) {

  std::vector<std::pair<Word, Byte>> testData = {
      {0xFF00, 0x00}, {0x0000, 0x00}, {0x00FF, 0xFF},
      {0xFFFF, 0xFF}, {0xFFF0, 0xF0}, {0xFF0F, 0x0F}};

  for (auto &[inVal, cmpVal] : testData) {
    BOOST_TEST(KeaBits::getLowByte(inVal) == cmpVal);
  }
}

BOOST_AUTO_TEST_CASE(high_byte_edge_cases) {

  std::vector<std::pair<Word, Byte>> testData = {
      {0xFF00, 0xFF}, {0x0000, 0x00}, {0x00FF, 0x00},
      {0xFFFF, 0xFF}, {0xF0FF, 0xF0}, {0x0FFF, 0x0F}};

  for (auto &[inVal, cmpVal] : testData) {
    BOOST_TEST(KeaBits::getHighByte(inVal) == cmpVal);
  }
}

BOOST_AUTO_TEST_CASE(join_from_split_property) {
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<Word> dist;

  for (size_t i = 0; i < trials; i++) {
    Word rsample = dist(eng);
    BOOST_TEST(KeaBits::wordFromBytes(KeaBits::getLowByte(rsample),
                                      KeaBits::getHighByte(rsample)) ==
               rsample);
  }
}

BOOST_AUTO_TEST_CASE(split_from_join_property) {
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<Byte> dist;

  for (size_t i = 0; i < trials; i++) {
    Byte loSample = dist(eng);
    Byte hiSample = dist(eng);
    BOOST_TEST(KeaBits::getLowByte(
                   KeaBits::wordFromBytes(loSample, hiSample)) == loSample);

    BOOST_TEST(KeaBits::getHighByte(
                   KeaBits::wordFromBytes(loSample, hiSample)) == hiSample);
  }
}

BOOST_AUTO_TEST_CASE(low_nibble_test) {
  for (int b = 0; b < BYTE_MAX; b++) {
    Byte lo = b & 0xF;
    BOOST_TEST(KeaBits::getLowNibble(b) == lo);
  }
}

BOOST_AUTO_TEST_CASE(high_nibble_test) {
  for (int b = 0; b < BYTE_MAX; b++) {
    Byte hi = b >> 4;
    BOOST_TEST(KeaBits::getHighNibble(b) == hi);
  }
}
