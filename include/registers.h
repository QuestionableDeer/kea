#ifndef KEA_REGISTERS_H
#define KEA_REGISTERS_H

#include "types.h"

#include <bitset>

class Registers {
// TODO: check register initialization
public:
  // compound register access is via functions
  void set_bc(Word value);
  void set_de(Word value);
  void set_hl(Word value);

  [[nodiscard]] auto get_bc() const -> Word;
  [[nodiscard]] auto get_de() const -> Word;
  [[nodiscard]] auto get_hl() const -> Word;

  // flag manipulation functions
  void set_zero_flag() { flags_.set(FlagNames::ZERO); }
  void clear_zero_flag() { flags_.reset(FlagNames::ZERO); }
  [[nodiscard]] auto get_zero_flag() const -> bool { return flags_[FlagNames::ZERO]; }

  void set_sub_flag() { flags_.set(FlagNames::SUB); }
  void clear_sub_flag() { flags_.reset(FlagNames::SUB); }
  [[nodiscard]] auto get_sub_flag() const -> bool { return flags_[FlagNames::SUB]; }

  void set_half_carry_flag() { flags_.set(FlagNames::HALF_CARRY); }
  void clear_half_carry_flag() { flags_.reset(FlagNames::HALF_CARRY); }
  [[nodiscard]] auto get_half_carry_flag() const -> bool { return flags_[FlagNames::HALF_CARRY]; }

  void set_carry_flag() { flags_.set(FlagNames::FULL_CARRY); }
  void clear_carry_flag() { flags_.reset(FlagNames::FULL_CARRY); }
  [[nodiscard]] auto get_carry_flag() const -> bool { return flags_[FlagNames::FULL_CARRY]; }

  // single registers are public data
  Byte a_={0x0};
  Byte b_={0x0};
  Byte c_={0x0};
  Byte d_={0x0};
  Byte e_={0x0};
  Byte h_={0x0};
  Byte l_={0x0};

  Word sp_={0x0};
  Word pc_={0x0};

private:
  enum FlagNames { ZERO = 0, SUB, HALF_CARRY, FULL_CARRY };
  static constexpr std::size_t flagCount = 4;
  std::bitset<flagCount> flags_;
};

#endif // KEA_REGISTERS_H
