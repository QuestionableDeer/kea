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
      resolve_block2_arithmetic(instruction);
      break;

    case 3:
      // TODO
      break;

    default:
      // not reachable
      break;
  }
}

auto InstructionSet::check_half_carry(const Byte op1, const Byte op2) -> bool {
  const Byte halfMax = 0xF;
  const Byte lo1 = KeaBits::getLowNibble(op1);
  const Byte lo2 = KeaBits::getLowNibble(op2);

  return (lo1 + lo2) > halfMax;
}

auto InstructionSet::check_full_carry(const Byte op1, const Byte op2) -> bool {
  const Byte max = 0xFF;
  const Byte remainder = max - op1;

  return op2 > remainder;
}

void InstructionSet::resolve_block2_arithmetic(const Byte instruction) {
  const Byte opMask = 0b0111;
  const Byte typeMask = 0b00111000;
  const Byte typeShift = 3;

  const Byte operation = (instruction & typeMask) >> typeShift;
  const Byte reg = instruction & opMask;

  switch (operation) {
    case 0:
      // add
      add_a_r8(reg);
      break;

    case 1:
      // adc
      break;

    case 2:
      // sub
      break;

    case 3:
      // sbc
      break;

    case 4:
      // and
      break;

    case 5:
      // xor
      break;

    case 6:
      // or
      break;

    case 7:
      // cp
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

void InstructionSet::add_a_r8(const Byte regId) {
  // add
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte addend = memory_.get_r8(regId);
  const Byte result = aVal + addend;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  }

  memory_.clear_sub_flag();

  if (check_half_carry(aVal, addend)) {
    memory_.set_half_carry_flag();
  }

  if (check_full_carry(aVal, addend)) {
    memory_.set_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}