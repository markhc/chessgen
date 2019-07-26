#pragma once

#include "config.hpp"

#if defined(__GNUC__) || defined(__GNUG__)
/**
 * Clang/LLVM.
 * Intel ICC/ICPC.
 * GNU GCC/G++.
 */
#define intrin_popcount __builtin_popcountll
#define intrin_forward_scan __builtin_ffsll
#define intrin_reverse_scan __builtin_clzll
#elif defined(_MSC_VER)
/**
 * Microsoft Visual Studio
 */
#include <intrin.h>

#define intrin_popcount _mm_popcnt_u64
inline int intrin_forward_scan(unsigned long x)
{
  unsigned long i = 0;
  _BitScanForward64(&i, x);
  return static_cast<int>(i);
}
inline int intrin_reverse_scan(unsigned long x)
{
  unsigned long i = 0;
  _BitScanReverse64(&i, x);
  return static_cast<int>(i);
}
#else
#error Sorry, this compiler is not supported.
#endif

#if defined(NDEBUG)
#define CG_ASSERT(x) ((void)0)
#else
#include <cassert>
#define CG_ASSERT(x) assert((x))
#endif
