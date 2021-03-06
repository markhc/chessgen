//
// Copyright (C) 2019-2019 markhc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include "config.hpp"

#if defined(__GNUC__) || defined(__GNUG__)
/**
 * Clang/LLVM.
 * Intel ICC/ICPC.
 * GNU GCC/G++.
 */
#define intrin_popcount __builtin_popcountll
#define intrin_ctz __builtin_ffsll
#define intrin_clz __builtin_clzll
#elif defined(_MSC_VER)
/**
 * Microsoft Visual Studio
 */
#include <intrin.h>

#define intrin_popcount _mm_popcnt_u64
inline int intrin_ctz(std::uint64_t x)
{
  unsigned long i;
  _BitScanForward64(&i, x);
  return static_cast<int>(i + 1);
}
inline int intrin_clz(std::uint64_t x)
{
  unsigned long i;
  _BitScanReverse64(&i, x);
  return 63 - static_cast<int>(i);
}
#else
#error Sorry, this compiler is not supported.
#endif

#if defined(NDEBUG)
#define CHESSGEN_ASSERT(x) ((void)0)
#else
#include <cassert>
#define CHESSGEN_ASSERT(x) assert((x))
#endif
