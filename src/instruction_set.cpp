/*
 * kea - a C++ Game Boy emulation program
 * Copyright (C) 2023 QuestionableDeer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "bits.h"
#include "instruction_set.h"

InstructionSet::InstructionSet(Memory& mem) : memory_(mem)
{
}

void InstructionSet::parse_and_execute(const Byte instruction) {
  const Byte selectShift = 6;
  const Byte selectBits = (instruction & selectMask) >> selectShift;
  
  switch (selectBits) {
    case 0:
      // TODO
      break;

    case 1:
      // register loads + halt
      load_r8_r8(instruction);
      break;

    case 2:
      // TODO
      break;

    case 3:
      // TODO
      break;

    default:
      // not reachable
      break;
  }
}

void InstructionSet::load_r8_r8(const Byte instruction) {
  const Byte srcMask = 0b0111;
  const Byte destMask = 0b011'1000;
  const Byte destShift = 3;

  const Byte source = instruction & srcMask;
  const Byte dest = (instruction & destMask) >> destShift;

  if (source == Memory::ByteRegisters::HL_MEM && 
      dest == Memory::ByteRegisters::HL_MEM) {
    halt();
    return;
  }

  memory_.set_r8(dest, memory_.get_r8(source));

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::halt() {
  // FIXME: figure out halting details when more system is complete
  memory_.pc++;
  instructionTimer_++;
}
