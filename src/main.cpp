#include "bits.h"
#include "types.h"

#include <iostream>

auto main() -> int {
  Word test = 0xAAFF;
  std::cout << std::hex << "Test word = 0x" << test << ", low = 0x"
            << static_cast<int>(KeaBits::getLowByte(test)) << std::hex
            << ",  high = 0x" << static_cast<int>(KeaBits::getHighByte(test))
            << std::hex << '\n';
  return 0;
}
