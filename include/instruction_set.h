#ifndef KEA_INSTRUCTION_SET_H
#define KEA_INSTRUCTION_SET_H

#include "types.h"

#include "memory.h"

class InstructionSet {
public:
  explicit InstructionSet(Memory& mem);

  void parse_and_execute(const Byte instruction);

private:
  [[nodiscard]] static auto check_half_carry(const Byte op1, const Byte op2) -> bool;
  [[nodiscard]] static auto check_full_carry(const Byte op1, const Byte op2) -> bool;
  void resolve_block2_arithmetic(const Byte instruction);

  // block 0 instructions
  // TODO

  // block 1 instructions
  void load_r8_r8(const Byte instruction);
  void halt();

  // block 2 instructions
  void add_a_r8(const Byte regId);

  // block 3 instructions
  // TODO

  constexpr static Byte selectMask = 0b1100'0000;

  Memory& memory_;
  uint64_t instructionTimer_={0};
};


#endif // KEA_INSTRUCTION_SET_H

