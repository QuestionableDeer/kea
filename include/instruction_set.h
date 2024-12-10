#ifndef KEA_INSTRUCTION_SET_H
#define KEA_INSTRUCTION_SET_H

#include "types.h"

#include "memory.h"

class InstructionSet {
public:
  explicit InstructionSet(Memory& mem);

  void parse_and_execute(const Byte instruction);

private:
  void load_r8_r8(const Byte instruction);
  void halt();

  constexpr static Byte selectMask = 0b1100'0000;

  Memory& memory_;
  uint64_t instructionTimer_={0};
};


#endif // KEA_INSTRUCTION_SET_H

