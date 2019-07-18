#pragma once

#include "bitboard.hpp"

namespace chessgen
{
namespace attacks
{
void     precomputeTables();
Bitboard getNonSlidingAttacks(Piece piece, Square from, Color color);
Bitboard getSlidingAttacks(Piece piece, Square from, Bitboard blockers);
}  // namespace attacks
}  // namespace chessgen
