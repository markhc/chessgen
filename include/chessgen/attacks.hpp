#pragma once

#include "bitboard.hpp"

namespace chessgen
{
namespace attacks
{
void     precomputeTables();
Bitboard getLineBetween(Square s1, Square s2);
Bitboard getNonSlidingAttacks(Piece piece, Square from, Color color);
Bitboard getSlidingAttacks(Piece piece, Square from, Bitboard blockers);
}  // namespace attacks
}  // namespace chessgen
