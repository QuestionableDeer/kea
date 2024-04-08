#ifndef KEA_BITS_H
#define KEA_BITS_H

#include "types.h"

#include <climits>

namespace KeaBits {

constexpr auto wordFromBytes(Byte lo, Byte hi) -> Word {
  Word result = hi;
  return (result << CHAR_BIT) | lo;
}

constexpr auto getLowByte(Word word) -> Byte { return (word & 0x00FF); }

constexpr auto getHighByte(Word word) -> Byte {
  return (word >> CHAR_BIT) & 0x00FF;
}

}; // namespace KeaBits

#endif // KEA_BITS_H
