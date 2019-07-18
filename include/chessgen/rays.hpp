#pragma once

#include "bitboard.hpp"

#include <array>

namespace chessgen
{
namespace rays
{
class Rays
{
public:
  auto operator[](Direction d) -> std::array<Bitboard, 64>& { return mRays[static_cast<int>(d)]; }
  auto operator[](Direction d) const -> std::array<Bitboard, 64> const&
  {
    return mRays[static_cast<int>(d)];
  }

private:
  std::array<std::array<Bitboard, 64>, static_cast<int>(Direction::Count)> mRays;
};

extern Rays _rays;

/**
 * @brief Precomputes the ray table
 */
void precomputeTables();

/**
 * @brief Retrieves the ray in the given direction for a square
 */
inline Bitboard getRayForSquare(Direction d, int square) { return _rays[d][square]; }
}  // namespace rays
}  // namespace chessgen
