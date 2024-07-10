#ifndef __DEFINES__
#define __DEFINES__

#include <iostream>

///when this is defined all the debug info's take effect
///#define DEBUG_PRINT  

//glossary
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;
using s32 = int;

///flags for setting flags
constexpr int C_f = (1 << 0);
constexpr int Z_f = (1 << 1);
constexpr int I_f = (1 << 2);
constexpr int D_f = (1 << 3);
constexpr int B_f = (1 << 4);
constexpr int V_f = (1 << 6);
constexpr int N_f = (1 << 7);

/// abomination
template<typename T, typename... Args>
void HEX_PRINT_TO_BE_WRAPPED(T&& _first, Args&&... _rest)
{
   std::cout<<std::hex;
   std::cout<<std::forward<T>(_first)<<" ";

   if constexpr(sizeof...(_rest) > 0)
   {
      HEX_PRINT_TO_BE_WRAPPED(std::forward<Args>(_rest)...);
   }
   else
   {
      std::cout<<std::dec;      
      std::cout<<"\n";
   }
}


#ifdef DEBUG_PRINT
#define HEX_PRINT(...) HEX_PRINT_TO_BE_WRAPPED(__VA_ARGS__, "|||| from function:", __FUNCTION__)
#else 
#define HEX_PRINT(...)
#endif


#endif
