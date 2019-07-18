#pragma once

#include "bitboard.hpp"

#include <array>

namespace chessgen
{
namespace rays
{
/**
 * @brief Precomputes the ray table
 */
void precomputeTables();

/**
 * @brief Retrieves the ray in the given direction for a square
 */
Bitboard getRayForSquare(Direction d, int square);
}  // namespace rays
}  // namespace chessgen
