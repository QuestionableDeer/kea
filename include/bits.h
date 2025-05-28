#ifndef KEA_BITS_H
#define KEA_BITS_H

#include <climits>

#include "types.h"

namespace KeaBits {

static constexpr Word loMask = 0x00FF;
static constexpr Byte loNibMask = 0xF;

constexpr auto wordFromBytes(Byte lo, Byte hi) -> Word {
  Word result = hi;
  return static_cast<Word>((result << CHAR_BIT) | lo);
}

constexpr auto getLowByte(Word word) -> Byte {
  return static_cast<Byte>(word & loMask);
}

constexpr auto getHighByte(Word word) -> Byte {
  return static_cast<Byte>((word >> CHAR_BIT) & loMask);
}

constexpr auto getLowNibble(Byte b) -> Byte { return (b & loNibMask); }

constexpr auto getHighNibble(Byte b) -> Byte { return (b >> (CHAR_BIT / 2)); }

};  // namespace KeaBits

#endif  // KEA_BITS_H
