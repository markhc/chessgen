#pragma once

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
#error Sorry, MSVC is not supported yet.
#else
#error Sorry, this compiler is not supported.
#endif

#if defined(NDEBUG)
#define CG_ASSERT(x) ((void)0)
#else
#include <cassert>
#define CG_ASSERT(x) assert((x))
#endif
