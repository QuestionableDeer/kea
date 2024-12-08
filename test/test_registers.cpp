#define BOOST_TEST_MODULE register_tests
#include <boost/test/included/unit_test.hpp>

#include <limits>

#include "bits.h"
#include "registers.h"
#include "types.h"

constexpr static int BYTE_MAX = static_cast<int>(std::numeric_limits<Byte>::max());

BOOST_AUTO_TEST_CASE(set_bc_register) {
  Registers reg;

  for (int lo = 0; lo <= BYTE_MAX; lo++) {
    for (int hi = 0; hi <= BYTE_MAX; hi++) {
      Word input = KeaBits::wordFromBytes(lo, hi);
      reg.set_bc(input);

      BOOST_TEST(reg.b == hi);
      BOOST_TEST(reg.c == lo);
      BOOST_TEST(reg.get_bc() == input);
    }
  }
}

BOOST_AUTO_TEST_CASE(set_de_register) {
  Registers reg;

  for (int lo = 0; lo <= BYTE_MAX; lo++) {
    for (int hi = 0; hi <= BYTE_MAX; hi++) {
      Word input = KeaBits::wordFromBytes(lo, hi);
      reg.set_de(input);

      BOOST_TEST(reg.d == hi);
      BOOST_TEST(reg.e == lo);
      BOOST_TEST(reg.get_de() == input);
    }
  }
}

BOOST_AUTO_TEST_CASE(set_hl_register) {
  Registers reg;

  for (int lo = 0; lo <= BYTE_MAX; lo++) {
    for (int hi = 0; hi <= BYTE_MAX; hi++) {
      Word input = KeaBits::wordFromBytes(lo, hi);
      reg.set_hl(input);

      BOOST_TEST(reg.h == hi);
      BOOST_TEST(reg.l == lo);
      BOOST_TEST(reg.get_hl() == input);
    }
  }
}

BOOST_AUTO_TEST_CASE(check_zero_flag) {
  Registers reg;

  reg.set_zero_flag();
  BOOST_TEST(reg.get_zero_flag());

  reg.clear_zero_flag();
  BOOST_TEST(!reg.get_zero_flag());
}

BOOST_AUTO_TEST_CASE(check_sub_flag) {
  Registers reg;

  reg.set_sub_flag();
  BOOST_TEST(reg.get_sub_flag());

  reg.clear_sub_flag();
  BOOST_TEST(!reg.get_sub_flag());
}

BOOST_AUTO_TEST_CASE(check_half_carry_flag) {
  Registers reg;

  reg.set_half_carry_flag();
  BOOST_TEST(reg.get_half_carry_flag());

  reg.clear_half_carry_flag();
  BOOST_TEST(!reg.get_half_carry_flag());
}

BOOST_AUTO_TEST_CASE(check_carry_flag) {
  Registers reg;

  reg.set_carry_flag();
  BOOST_TEST(reg.get_carry_flag());

  reg.clear_carry_flag();
  BOOST_TEST(!reg.get_carry_flag());
}
