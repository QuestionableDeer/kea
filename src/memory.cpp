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

#include "memory.h"

#include <algorithm>
#include <iterator>

#include "bits.h"

#ifdef MEM_DEBUG
#include <cstdlib>
#include <iostream>
#endif

Memory::Memory() : memory_() {
  memory_.fill(0x00);
  byteRegisters_.fill(0x0);
}

auto Memory::fetchByte(Word addr) const -> Byte {
  // TODO: add block logic for detailed memory map
#ifdef MEM_DEBUG
  Byte retval;
  try {
    retval = memory_.at(addr);
  } catch (std::out_of_range const &err) {
    std::cerr << "fetchByte() invalid memory access: " << err.what();
    std::exit(EXIT_FAILURE);
  }

  return retval;
#else
  return memory_[addr];
#endif
}

void Memory::setByte(Word addr, Byte val) {
  // TODO: add block logic for detailed memory map

#ifdef MEM_DEBUG
  try {
    memory_.at(addr) = val;
  } catch (std::out_of_range const &err) {
    std::cerr << "setByte invalid memory access: " << err.what();
    std::exit(EXIT_FAILURE);
  }

#else
  memory_[addr] = val;
#endif
}

auto Memory::fetchWord(Word addr) const -> Word {
  // TODO: add block logic for detailed memory map

#ifdef MEM_DEBUG
  Word result;
  try {
    Byte lo = memory_.at(addr);
    Byte hi = memory_.at(addr + 1);
    result = KeaBits::wordFromBytes(lo, hi);
  } catch (std::out_of_range const &err) {
    std::cerr << "fetchWord invalid memory access: " << err.what();
    std::exit(EXIT_FAILURE);
  }

  return result;
#else
  Byte lo = memory_[addr];
  Byte hi = memory_[addr + 1];

  return KeaBits::wordFromBytes(lo, hi);
#endif
}

void Memory::setWord(Word addr, Word val) {
  // TODO: add block logic for detailed memory map

  Byte const lo = KeaBits::getLowByte(val);
  Byte const hi = KeaBits::getHighByte(val);

#ifdef MEM_DEBUG
  try {
    memory_.at(addr) = lo;
    memory_.at(addr + 1) = hi;
  } catch (std::out_of_range const &err) {
    std::cerr << "setWord invalid memory access: " << err.what();
    std::exit(EXIT_FAILURE);
  }
#else
  memory_[addr] = lo;
  memory_[addr + 1] = hi;
#endif
}

void Memory::loadRom(std::istream &rom) {
  // TODO: figure out how ROM banking works with input files

  // since this is an array of bytes that C++ treats as chars, we need to avoid
  // parsing bytes that overlap with ASCII whitespace as newlines, etc.
  rom.unsetf(std::ios::skipws);
  std::copy_n(std::istream_iterator<Byte>(rom), memSize, memory_.begin());
  rom.setf(std::ios::skipws);
}

void Memory::dumpRom(std::ostream &rom) {
  // TODO: figure out ROM banking

  // since this is an array of bytes that C++ treats as chars, we need to avoid
  // parsing bytes that overlap with ASCII whitespace as newlines, etc.
  rom.unsetf(std::ios::skipws);
  std::copy(memory_.begin(), memory_.end(), std::ostream_iterator<Byte>(rom));
  rom.setf(std::ios::skipws);
}

void Memory::set_r8(const Byte id, Byte value) {
  if (id == ByteRegisters::HL_MEM) {
    const Byte lo = byteRegisters_[ByteRegisters::L];
    const Byte hi = byteRegisters_[ByteRegisters::H];
    Word addr = KeaBits::wordFromBytes(lo, hi);

    memory_[addr] = value;
    return;
  }

  byteRegisters_[id] = value;
}

auto Memory::get_r8(const Byte id) const -> Byte {
  // TODO: this will need to be broken down into sub-states
  // for accurate timing later
  if (id == ByteRegisters::HL_MEM) {
    const Byte lo = byteRegisters_[ByteRegisters::L];
    const Byte hi = byteRegisters_[ByteRegisters::H];
    Word addr = KeaBits::wordFromBytes(lo, hi);

    return memory_[addr];
  }

  return byteRegisters_[id];
}

void Memory::set_r16(const Byte id, Word value) {
  switch (id) {
    case 0:
      // r16 = BC
      byteRegisters_[ByteRegisters::B] = KeaBits::getHighByte(value);
      byteRegisters_[ByteRegisters::C] = KeaBits::getLowByte(value);
      break;

    case 1:
      // r16 = DE
      byteRegisters_[ByteRegisters::D] = KeaBits::getHighByte(value);
      byteRegisters_[ByteRegisters::E] = KeaBits::getLowByte(value);
      break;

    case 2:
      // r16 = HL
      byteRegisters_[ByteRegisters::H] = KeaBits::getHighByte(value);
      byteRegisters_[ByteRegisters::L] = KeaBits::getLowByte(value);
      break;

    case 3:
      // r16 = SP
      sp = value;
      break;

    default:
      throw std::runtime_error("Invalid r16 id");
  }
}

auto Memory::get_r16(const Byte id) const -> Word {
  Word value = 0;

  switch (id) {
    case 0:
      // r16 = BC
      value = KeaBits::wordFromBytes(byteRegisters_[ByteRegisters::C],
                                     byteRegisters_[ByteRegisters::B]);
      break;

    case 1:
      // r16 = DE
      value = KeaBits::wordFromBytes(byteRegisters_[ByteRegisters::E],
                                     byteRegisters_[ByteRegisters::D]);
      break;

    case 2:
      // r16 = HL
      value = KeaBits::wordFromBytes(byteRegisters_[ByteRegisters::L],
                                     byteRegisters_[ByteRegisters::H]);
      break;

    case 3:
      // r16 = SP
      value = sp;
      break;

    default:
      throw std::runtime_error("Invalid r16 id");
  }

  return value;
}

// static
auto Memory::get_r8_from_op(const Byte instruction) -> Byte {
  const Byte regMask = 0b011'1000;
  const Byte regShift = 3;
  return (instruction & regMask) >> regShift;
}

// static
auto Memory::get_r16_from_op(const Byte instruction) -> Byte {
  const Byte regMask = 0b0011'0000;
  const Byte regShift = 4;
  return (instruction & regMask) >> regShift;
}
