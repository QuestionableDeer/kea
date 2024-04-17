#define BOOST_TEST_MODULE bit_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "types.h"

#include <utility>
#include <vector>

// namespace bdata = boost::unit_test::data;

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

// BOOST_DATA_TEST_CASE(
//     property_test,
//     bdata::random(bdata::distribution =
//                       std::uniform_int_distribution<Word>(0x0000, 0xFFFF)) ^
//         bdata::xrange(1000),
//     rsample, index) {
//
//   BOOST_TEST(KeaBits::wordFromBytes(KeaBits::getLowByte(rsample),
//                                     KeaBits::getHighByte(rsample)) ==
//                                     rsample);
// }
