#include "chessgen/rays.hpp"

namespace chessgen
{
namespace rays
{
static constexpr Bitboard moveWest(Bitboard b, int n)
{
  auto newbb = b;
  for (int i = 0; i < n; i++) {
    newbb = ((newbb >> 1) & (~Bitboards::FileH));
  }
  return newbb;
}
static constexpr Bitboard moveEast(Bitboard b, int n)
{
  auto newbb = b;
  for (int i = 0; i < n; i++) {
    newbb = ((newbb << 1) & (~Bitboards::FileA));
  }
  return newbb;
}

class Rays
{
public:
  auto&       operator[](Direction d) { return mRays[to_int(d)]; }
  auto const& operator[](Direction d) const { return mRays[to_int(d)]; }

private:
  Bitboard mRays[to_int(Direction::Count)][64];
};

/**
 * @brief Precomputed ray table
 */
static Rays _rays;

void precomputeTables()
{
  for (auto sq = 0; sq < 64; ++sq) {
    auto const file = static_cast<int>(getFile(squareFromIndex(sq)));
    auto const rank = static_cast<int>(getRank(squareFromIndex(sq)));
    // clang-format off
    _rays[Direction::North][sq]     = 0x0101010101010100ULL << sq;
    _rays[Direction::South][sq]     = 0x0080808080808080ULL >> (63 - sq);
    _rays[Direction::East][sq]      = 2 * ((1ULL << (sq | 7)) - (1ULL << sq));
    _rays[Direction::West][sq]      = (1ULL << sq) - (1ULL << (sq & 56));
    _rays[Direction::NorthEast][sq] = moveEast(0x8040201008040200ULL,     file) << (     rank  * 8);
    _rays[Direction::SouthEast][sq] = moveEast(0x0002040810204080ULL,     file) >> ((7 - rank) * 8);
    _rays[Direction::NorthWest][sq] = moveWest(0x0102040810204000ULL, 7 - file) << (     rank  * 8);
    _rays[Direction::SouthWest][sq] = moveWest(0x0040201008040201ULL, 7 - file) >> ((7 - rank) * 8);
    // clang-format on
  }
}

Bitboard getRayForSquare(Direction d, int square) { return _rays[d][square]; }
}  // namespace rays
}  // namespace chessgen
