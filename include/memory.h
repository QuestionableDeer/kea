#ifndef KEA_MEMORY_H
#define KEA_MEMORY_H

#include "types.h"

#include <array>
#include <bitset>
#include <fstream>

class Memory {
public:
  static constexpr std::size_t memSize = 0x10000;

  Memory();

  // memory accesses are bounds-checked when MEM_DEBUG = TRUE

  [[nodiscard]] auto fetchByte(Word addr) const -> Byte;
  void setByte(Word addr, Byte val);

  // Note: Game Boy memory is little-endian
  [[nodiscard]] auto fetchWord(Word addr) const -> Word;
  void setWord(Word addr, Word val);

  void loadRom(std::istream &rom);
  void dumpRom(std::ostream &rom);

  // register stuff
  enum ByteRegisters {B, C, D, E, H, L, HL_MEM, A};
  enum WordRegisters {BC, DE, HL, SP};

  void set_r8(const Byte id, Byte value);
  [[nodiscard]] auto get_r8(const Byte id) const -> Byte;

  void set_r16(const Byte id, Word value);
  [[nodiscard]] auto get_r16(const Byte id) const -> Word;

  // TODO: add r16stk and r16mem orders?

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

  Word sp={0x0};
  Word pc={0x0};

private:
  std::array<Byte, memSize> memory_;

  static constexpr std::size_t regSlots = 8;
  std::array<Byte, regSlots> byteRegisters_;

  enum FlagNames { ZERO = 0, SUB, HALF_CARRY, FULL_CARRY };
  static constexpr std::size_t flagCount = 4;
  std::bitset<flagCount> flags_;
};

#endif // KEA_MEMORY_H
