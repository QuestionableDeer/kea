#ifndef KEA_MEMORY_H
#define KEA_MEMORY_H

#include "types.h"

#include <array>
#include <fstream>

class Memory {
public:
  static constexpr memSize = 0x10000;

  Memory();

  // memory accesses are bounds-checked when MEM_DEBUG = TRUE

  [[nodiscard]] auto fetchByte(Word addr) const -> Byte;
  void setByte(Word addr, Byte val);

  // Note: Game Boy memory is little-endian
  [[nodiscard]] auto fetchWord(Word addr) const -> Word;
  void setWord(Word addr, Word val);

  void loadRom(std::istream &rom);
  void dumpRom(std::ostream &rom);

private:
  std::array<Byte, memSize> memory_;
};

#endif // KEA_MEMORY_H
