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
#include "bits.h"

#include <algorithm>
#include <iterator>

#ifdef MEM_DEBUG
#include <cstdlib>
#include <iostream>
#endif

Memory::Memory() : memory_() {
  // TODO: check register initializations
  memory_.fill(0x00);
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
