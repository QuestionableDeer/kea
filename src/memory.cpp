/*
 * kea - a C++ Chip-8 emulation program
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

#endif
}

auto Memory::fetchWord(Word addr) const -> Word {
  // TODO: add block logic for detailed memory map

  std::ignore = addr;
#ifdef MEM_DEBUG

#else
  return 0xBADD;
#endif
}

void Memory::setWord(Word addr, Word val) {
  // TODO: add block logic for detailed memory map

  std::ignore = addr;
  std::ignore = val;
#ifdef MEM_DEBUG

#else

#endif
}

void Memory::loadRom(std::istream &rom) {
  // TODO: implement
  std::ignore = rom;
}

void Memory::dumpRom(std::ostream &rom) {
  // TODO: implement
  std::ignore = rom;
}
