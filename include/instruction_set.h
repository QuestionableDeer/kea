#ifndef KEA_INSTRUCTION_SET_H
#define KEA_INSTRUCTION_SET_H

#include "types.h"

#include "memory.h"

#include <functional>

class InstructionSet {
public:
  explicit InstructionSet(Memory& mem);

  void parse_and_execute(const Byte instruction);

private:
  [[nodiscard]] static auto check_half_carry(const Byte op1, const Byte op2, const Byte carry) -> bool;
  [[nodiscard]] static auto check_full_carry(const Word op1, const Word op2, const Word carry) -> bool;
  [[nodiscard]] static auto check_half_borrow(const Byte op1, const Byte op2, const Byte carry) -> bool;
  [[nodiscard]] static auto check_full_borrow(const Word op1, const Word op2, const Word carry) -> bool;
  void resolve_block2_arithmetic(const Byte instruction);

  // block 0 instructions
  // TODO

  // block 1 instructions
  void load_r8_r8(const Byte instruction);
  void halt();

  // block 2 instructions
  void add_a_r8(const Byte regId);
  void adc_a_r8(const Byte regId);
  void sub_a_r8(const Byte regId);
  void sbc_a_r8(const Byte regId);
  void and_a_r8(const Byte regId);
  void xor_a_r8(const Byte regId);
  void or_a_r8(const Byte regId);
  void cp_a_r8(const Byte regId);

  // block 3 instructions
  // TODO

  constexpr static Byte selectMask = 0b1100'0000;

  Memory& memory_;
  uint64_t instructionTimer_={0};
};


#endif // KEA_INSTRUCTION_SET_H

