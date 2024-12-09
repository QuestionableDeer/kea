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

#include "registers.h"
#include "bits.h"

#include <cassert>
#include <stdexcept>

Registers::Registers() : byteRegisters_() {
  // TODO check register initialization
  byteRegisters_.fill(0x0);
}

void Registers::set_r8(const Byte id, Byte value) {
  if (id == 6) {
    throw std::runtime_error("Invalid r8 id");
  }

  byteRegisters_[id] = value;
}

auto Registers::get_r8(const Byte id) const -> Byte {
  if (id == 6) {
    throw std::runtime_error("Invalid r8 id");
  }

  return byteRegisters_[id];
}

void Registers::set_r16(const Byte id, Word value) {
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

auto Registers::get_r16(const Byte id) const -> Word {
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
