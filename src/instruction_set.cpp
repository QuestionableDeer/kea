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

#include "instruction_set.h"

#include <bit>
#include <cassert>

#include "bits.h"
#include "types.h"

InstructionSet::InstructionSet(Memory &mem) : memory_(mem) {}

void InstructionSet::parse_and_execute(const Byte instruction) {
  const Byte selectShift = 6;
  const Byte selectBits = (instruction & selectMask) >> selectShift;

  switch (selectBits) {
    case 0:
      // TODO
      resolve_block0_calls(instruction);
      break;

    case 1:
      // register loads + halt
      load_r8_r8(instruction);
      break;

    case 2:
      // A-register arithmetic
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

// static
auto InstructionSet::check_half_carry(const Byte op1, const Byte op2,
                                      const Byte carry) -> bool {
  const Byte halfMax = 0xF;
  const Byte lo1 = KeaBits::getLowNibble(op1);
  const Byte lo2 = KeaBits::getLowNibble(op2);

  return (lo1 + lo2 + carry) > halfMax;
}

// static
auto InstructionSet::check_full_carry(const Word op1, const Word op2,
                                      const Word carry) -> bool {
  const Word max = 0xFF;
  return (op1 + op2 + carry) > max;
}

// static
auto InstructionSet::check_half_borrow(const Byte op1, const Byte op2,
                                       const Byte carry) -> bool {
  const Byte lo1 = KeaBits::getLowNibble(op1);
  const Byte lo2 = KeaBits::getLowNibble(op2);

  return (lo2 + carry) > lo1;
}

// static
auto InstructionSet::check_full_borrow(const Word op1, const Word op2,
                                       const Word carry) -> bool {
  return (op2 + carry) > op1;
}

void InstructionSet::resolve_block0_calls(const Byte instruction) {
  const Byte nopCode = 0;
  const Byte stopCode = 0b00010000;

  const Byte bitwiseCode = 0b111;
  const Byte jrCode = 0b000;
  const Byte ldCode = 0b110;

  const Byte lsbTriple = instruction & 0b111;
  const Byte bitTwo = instruction & 0b100;

  if (instruction == nopCode) {
    nop();
  } else if (instruction == stopCode) {
    stop();
  } else if (lsbTriple == bitwiseCode) {
    resolve_block0_bitwise_op(instruction);
  } else if (lsbTriple == jrCode) {
    resolve_jr_call(instruction);
  } else if (lsbTriple == ldCode) {
    resolve_imm8_load(instruction);
  }

  // remaining ops have to be filtered by low bits
  const auto ilow = KeaBits::getLowNibble(instruction);
  switch (ilow) {
    case 0b0001:
      ld_r16_imm16(instruction);
      break;

    case 0b0010:
      ld_r16mem_a(instruction);
      break;

    case 0b1010:
      ld_a_r16mem(instruction);
      break;

    case 0b1000:
      ld_imm16_sp(instruction);
      break;

    case 0b0011:
      inc_r16(instruction);
      break;

    case 0b1011:
      dec_r16(instruction);
      break;

    case 0b1001:
      add_hl_r16(instruction);
      break;

    default:
      // should not be reachable
      assert(false && "invalid block 0 instruction code");
      break;
  }
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
      adc_a_r8(reg);
      break;

    case 2:
      // sub
      sub_a_r8(reg);
      break;

    case 3:
      // sbc
      sbc_a_r8(reg);
      break;

    case 4:
      // and
      and_a_r8(reg);
      break;

    case 5:
      // xor
      xor_a_r8(reg);
      break;

    case 6:
      // or
      or_a_r8(reg);
      break;

    case 7:
      // cp
      cp_a_r8(reg);
      break;

    default:
      // should not be reachable
      assert(false && "invalid block 2 arithmetic instruction");
      break;
  }
}

void InstructionSet::nop() {
  // do nothing but spin for one cycle
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::stop() {
  // FIXME: need to understand the subtleties for this instruction
}

void InstructionSet::resolve_block0_bitwise_op(const Byte instruction) {
  const Byte shift = 3;
  const Byte opIndex = instruction >> shift;

  switch (opIndex) {
    case 0:
      rlca();
      break;

    case 1:
      rrca();
      break;

    case 2:
      rla();
      break;

    case 3:
      rra();
      break;

    case 4:
      daa();
      break;

    case 5:
      cpl();
      break;

    case 6:
      scf();
      break;

    case 7:
      ccf();
      break;

    default:
      // should not be reached
      assert(false && "invalid block0 bitwise instruction");
      break;
  }
}

void InstructionSet::resolve_jr_call(const Byte instruction) {}

void InstructionSet::resolve_imm8_load(const Byte instruction) {}

void InstructionSet::rlca() {
  // rotate register A left

  // first carry flag is set to high bit
  const Byte mask = 0x80;
  Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);

  if (aVal & mask) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // rotate
  aVal = std::rotl(aVal, 1);
  memory_.set_r8(Memory::ByteRegisters::A, aVal);

  // set flags
  memory_.clear_zero_flag();
  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::rrca() {
  // rotate register A right

  // first carry flag is set to low bit
  const Byte mask = 0x1;
  Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);

  if (aVal & mask) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // rotate
  aVal = std::rotr(aVal, 1);
  memory_.set_r8(Memory::ByteRegisters::A, aVal);

  // set flags
  memory_.clear_zero_flag();
  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::rla() {
  // rotate A left through the carry flag

  // record high bit
  const Byte hiMask = 0x80;
  Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);

  // shift A
  const Byte hiBitOnly = aVal & hiMask;
  aVal <<= 1;

  // rotate carry flag into low bit if set
  if (memory_.get_carry_flag() != 0) {
    aVal |= 0b1;
  }

  // set carry flag
  if (hiBitOnly != 0) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // set remaining flags
  memory_.clear_zero_flag();
  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::rra() {
  // rotate A right through the carry flag

  // record low bit
  const Byte loMask = 0x1;
  Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);

  // shift A
  const Byte loBitOnly = aVal & loMask;
  aVal >>= 1;

  // rotate carry flag into high bit if set
  if (memory_.get_carry_flag() != 0) {
    aVal |= 0x80;
  }

  // set carry flag
  if (loBitOnly != 0) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // set remaining flags
  memory_.clear_zero_flag();
  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::daa() {
  // FIXME: need to understand BCD better
}

void InstructionSet::cpl() {
  // complement register A
  memory_.set_r8(Memory::ByteRegisters::A,
                 ~memory_.get_r8(Memory::ByteRegisters::A));

  memory_.set_sub_flag();
  memory_.set_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::scf() {
  // set carry flag
  memory_.set_carry_flag();

  memory_.clear_zero_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::ccf() {
  // complement carry flag
  memory_.flip_carry_flag();

  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();

  memory_.pc++;
  instructionTimer_++;
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
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal + bVal;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.clear_sub_flag();

  // if we con't clear carry flags here, they could mess up chained ops?
  const Byte carry = 0;
  if (check_half_carry(aVal, bVal, carry)) {
    memory_.set_half_carry_flag();
  } else {
    memory_.clear_half_carry_flag();
  }

  if (check_full_carry(aVal, bVal, carry)) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::adc_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte carry = memory_.get_carry_flag() ? 1 : 0;
  Byte result = aVal + bVal;
  result += carry;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.clear_sub_flag();

  // if we con't clear carry flags here, they could mess up chained ops?
  if (check_half_carry(aVal, bVal, carry)) {
    memory_.set_half_carry_flag();
  } else {
    memory_.clear_half_carry_flag();
  }

  if (check_full_carry(aVal, bVal, carry)) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::sub_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal - bVal;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.set_sub_flag();

  // if we con't clear carry flags here, they could mess up chained ops?
  const Byte carry = 0;
  if (check_half_borrow(aVal, bVal, carry)) {
    memory_.set_half_carry_flag();
  } else {
    memory_.clear_half_carry_flag();
  }

  if (check_full_borrow(aVal, bVal, carry)) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::sbc_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte carry = memory_.get_carry_flag() ? 1 : 0;
  Byte result = aVal - bVal;
  result -= carry;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.set_sub_flag();

  // if we con't clear carry flags here, they could mess up chained ops?
  if (check_half_borrow(aVal, bVal, carry)) {
    memory_.set_half_carry_flag();
  } else {
    memory_.clear_half_carry_flag();
  }

  if (check_full_borrow(aVal, bVal, carry)) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::and_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal & bVal;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.clear_sub_flag();
  memory_.set_half_carry_flag();
  memory_.clear_carry_flag();

  // increment counters
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::xor_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal ^ bVal;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();
  memory_.clear_carry_flag();

  // increment counters
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::or_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal | bVal;

  memory_.set_r8(Memory::ByteRegisters::A, result);

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.clear_sub_flag();
  memory_.clear_half_carry_flag();
  memory_.clear_carry_flag();

  // increment counters
  memory_.pc++;
  instructionTimer_++;
}

void InstructionSet::cp_a_r8(const Byte regId) {
  // obtain result
  const Byte aVal = memory_.get_r8(Memory::ByteRegisters::A);
  const Byte bVal = memory_.get_r8(regId);
  const Byte result = aVal - bVal;

  // update flags
  if (result == 0) {
    memory_.set_zero_flag();
  } else {
    memory_.clear_zero_flag();
  }

  memory_.set_sub_flag();

  // if we con't clear carry flags here, they could mess up chained ops?
  const Byte carry = 0;
  if (check_half_borrow(aVal, bVal, carry)) {
    memory_.set_half_carry_flag();
  } else {
    memory_.clear_half_carry_flag();
  }

  if (check_full_borrow(aVal, bVal, carry)) {
    memory_.set_carry_flag();
  } else {
    memory_.clear_carry_flag();
  }

  // increment cycles and PC by 1
  memory_.pc++;
  instructionTimer_++;
}
