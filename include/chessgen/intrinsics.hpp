#pragma once

#include <type_traits>

#include "platform.hpp"

namespace chessgen
{
/**
 * @brief Returns the index of the least significant non-zero bit or -1 if no bit is set
 */
inline int bitscan_forward(std::uint64_t value)
{
  if (value == 0ULL) {
    return -1;
  }

  return intrin_forward_scan(value) - 1;
}

/**
 * @brief Returns the index of the most significant non-zero bit or -1 if no bit is set
 */
inline int bitscan_reverse(std::uint64_t value)
{
  if (value == 0ULL) {
    return -1;
  }

  return 63 - intrin_reverse_scan(value);
}

/**
 * @brief Sreturns true if more than 1 bit is set
 */
constexpr bool more_than_one(Bitboard b) { return b & (b - 1); };

/**
 * @brief Sets the least significant non-zero bit to 0 and returns its index.
 */
inline int pop_lsb(std::uint64_t &value)
{
  int lsbIndex = intrin_forward_scan(value) - 1;
  value &= value - 1;
  return lsbIndex;
}

/**
 * @brief Returns the number of non-zero bits in the given value.
 */
inline int pop_count(std::uint64_t value) { return intrin_popcount(value); }

inline int lsb(std::uint64_t b)
{
  CG_ASSERT(b);
  return intrin_count_trailing_zeroes(b);
}

inline int msb(std::uint64_t b)
{
  CG_ASSERT(b);
  return 63 ^ intrin_count_leading_zeroes(b);
}

}  // namespace chessgen
