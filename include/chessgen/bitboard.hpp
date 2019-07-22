#pragma once

#include <cstdint>
#include <ostream>
#include "platform.hpp"
#include "types.hpp"

namespace chessgen
{
using Bitboard = std::uint64_t;

namespace Bitboards
{
constexpr Bitboard AllSquares  = ~Bitboard(0);
constexpr Bitboard DarkSquares = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard FileA       = 0x0101010101010101ULL;
constexpr Bitboard FileB       = FileA << 1;
constexpr Bitboard FileC       = FileA << 2;
constexpr Bitboard FileD       = FileA << 3;
constexpr Bitboard FileE       = FileA << 4;
constexpr Bitboard FileF       = FileA << 5;
constexpr Bitboard FileG       = FileA << 6;
constexpr Bitboard FileH       = FileA << 7;
constexpr Bitboard Rank1       = 0xFF;
constexpr Bitboard Rank2       = Rank1 << (8 * 1);
constexpr Bitboard Rank3       = Rank1 << (8 * 2);
constexpr Bitboard Rank4       = Rank1 << (8 * 3);
constexpr Bitboard Rank5       = Rank1 << (8 * 4);
constexpr Bitboard Rank6       = Rank1 << (8 * 5);
constexpr Bitboard Rank7       = Rank1 << (8 * 6);
constexpr Bitboard Rank8       = Rank1 << (8 * 7);
}  // namespace Bitboards

extern std::uint64_t SquareBB[64];

constexpr void BBSetBit(Bitboard& bb, Square s) { bb |= 1ULL << static_cast<int>(s); }
constexpr void BBClearBit(Bitboard& bb, Square s) { bb &= ~(1ULL << static_cast<int>(s)); }

constexpr Bitboard BBShift(Bitboard bb, Direction d)
{
  switch (d) {
    case Direction::North:
      return bb << 8;
    case Direction::South:
      return bb >> 8;
    case Direction::East:
      return (bb & ~Bitboards::FileH) << 1;
    case Direction::West:
      return (bb & ~Bitboards::FileA) >> 1;
    case Direction::NorthEast:
      return (bb & ~Bitboards::FileH) << 9;
    case Direction::NorthWest:
      return (bb & ~Bitboards::FileA) << 7;
    case Direction::SouthEast:
      return (bb & ~Bitboards::FileH) >> 7;
    case Direction::SouthWest:
      return (bb & ~Bitboards::FileA) >> 9;
    case Direction::Count:
    default:
      return 0;
  }
}

inline Bitboard  operator&(Bitboard b, Square s) { return b & SquareBB[indexFromSquare(s)]; }
inline Bitboard  operator|(Bitboard b, Square s) { return b | SquareBB[indexFromSquare(s)]; }
inline Bitboard  operator^(Bitboard b, Square s) { return b ^ SquareBB[indexFromSquare(s)]; }
inline Bitboard& operator|=(Bitboard& b, Square s) { return b |= SquareBB[indexFromSquare(s)]; }
inline Bitboard& operator^=(Bitboard& b, Square s) { return b ^= SquareBB[indexFromSquare(s)]; }

Bitboard BBGetBetween(Square s1, Square s2);
void     BBPrettyPrint(std::ostream& os, Bitboard bb);
}  // namespace chessgen
