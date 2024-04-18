#include "bits.h"
#include "memory.h"
#include "types.h"

#include <iostream>

auto main() -> int {
  Word test = 0xAAFF;
  std::cout << std::hex << "Test word = 0x" << test << ", low = 0x"
            << static_cast<int>(KeaBits::getLowByte(test)) << std::hex
            << ",  high = 0x" << static_cast<int>(KeaBits::getHighByte(test))
            << std::hex << '\n';

  Memory memTest;

  Word addr = 0x1234;
  Byte val = 0xAF;
  memTest.setByte(addr, val);

  std::cout << std::hex << "Test memory at address 0x" << addr << " = 0x"
            << static_cast<int>(memTest.fetchByte(addr)) << '\n';
  return 0;
}
