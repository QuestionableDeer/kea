#define BOOST_TEST_MODULE bit_tests
#include <boost/test/included/unit_test.hpp>

#include "bits.h"
#include "types.h"

BOOST_AUTO_TEST_CASE(low_byte_zero) {
  Word tval = 0xFF00;
  Byte expect = 0x00;
  BOOST_TEST(KeaBits::getLowByte(tval) == expect);
}

BOOST_AUTO_TEST_CASE(high_byte_zero) {
  Word tval = 0x00FF;
  Byte expect = 0x00;
  BOOST_TEST(KeaBits::getHighByte(tval) == expect);
}
