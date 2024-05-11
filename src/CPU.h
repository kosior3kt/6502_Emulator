#ifndef __CPU__
#define __CPU__

#include "memory.h"

struct CPU
{

      Word PC; /// program counter
      Word SP; /// stack pointer

      Byte A, X, Y; /// registers

      Byte C : 1; // status flag
      Byte Z : 1; // status flag
      Byte I : 1; // status flag
      Byte D : 1; // status flag
      Byte B : 1; // status flag
      Byte V : 1; // status flag
      Byte N : 1; // status flag

      static constexpr Byte INS_LDA_IM  = 0xA9;
      static constexpr Byte INS_LDA_ZP  = 0xA5;
      static constexpr Byte INS_LDA_ZPX = 0xB5;
      static constexpr Byte INS_JSR     = 0x20;

      void Reset(Mem &_mem);

      Byte FetchByte(u32 &_cycles, const Mem &_mem);

      Word FetchWord(u32 &_cycles, const Mem &_mem);

      Byte ReadByte(u32 &_cycles, const Byte &_addr, const Mem &_mem);

      void LDASetStatus();

      void Execute(u32 _cycles, Mem &_mem);
};

#endif
