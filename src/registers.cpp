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

void Registers::set_bc(Word value) {
  b = KeaBits::getHighByte(value);
  c = KeaBits::getLowByte(value);
}

void Registers::set_de(Word value) {
  d = KeaBits::getHighByte(value);
  e = KeaBits::getLowByte(value);
}

void Registers::set_hl(Word value) {
  h = KeaBits::getHighByte(value);
  l = KeaBits::getLowByte(value);
}

auto Registers::get_bc() const -> Word {
  return KeaBits::wordFromBytes(c, b);
}

auto Registers::get_de() const -> Word {
  return KeaBits::wordFromBytes(e, d);
}

auto Registers::get_hl() const -> Word {
  return KeaBits::wordFromBytes(l, h);
}
