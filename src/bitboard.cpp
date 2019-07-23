#include "chessgen/bitboard.hpp"
#include "chessgen/attacks.hpp"

namespace chessgen
{
std::uint64_t SquareBB[64] = {
    1ULL << 0,  1ULL << 1,  1ULL << 2,  1ULL << 3,  1ULL << 4,  1ULL << 5,  1ULL << 6,  1ULL << 7,
    1ULL << 8,  1ULL << 9,  1ULL << 10, 1ULL << 11, 1ULL << 12, 1ULL << 13, 1ULL << 14, 1ULL << 15,
    1ULL << 16, 1ULL << 17, 1ULL << 18, 1ULL << 19, 1ULL << 20, 1ULL << 21, 1ULL << 22, 1ULL << 23,
    1ULL << 24, 1ULL << 25, 1ULL << 26, 1ULL << 27, 1ULL << 28, 1ULL << 29, 1ULL << 30, 1ULL << 31,
    1ULL << 32, 1ULL << 33, 1ULL << 34, 1ULL << 35, 1ULL << 36, 1ULL << 37, 1ULL << 38, 1ULL << 39,
    1ULL << 40, 1ULL << 41, 1ULL << 42, 1ULL << 43, 1ULL << 44, 1ULL << 45, 1ULL << 46, 1ULL << 47,
    1ULL << 48, 1ULL << 49, 1ULL << 50, 1ULL << 51, 1ULL << 52, 1ULL << 53, 1ULL << 54, 1ULL << 55,
    1ULL << 56, 1ULL << 57, 1ULL << 58, 1ULL << 59, 1ULL << 60, 1ULL << 61, 1ULL << 62, 1ULL << 63,
};
Bitboard BBGetBetween(Square s1, Square s2)
{
  return attacks::getLineBetween(s1, s2) &
         ((Bitboards::AllSquares << (makeIndex(s1) + (makeIndex(s1) < makeIndex(s2)))) ^
          (Bitboards::AllSquares << (makeIndex(s2) + !(makeIndex(s1) < makeIndex(s2)))));
}
void BBPrettyPrint(std::ostream& os, Bitboard bb)
{
  os << "  +-----------------+\n";
  for (Rank r = Rank::Rank8; r >= Rank::Rank1; --r) {
    os << static_cast<int>(r) + 1 << " | ";
    for (File f = File::FileA; f <= File::FileH; ++f) {
      os << ((bb & makeSquare(f, r)) != 0 ? "x " : ". ");
    }

    os << "|\n";
  }

  os << "  +-----------------+\n";
  os << "    A B C D E F G H" << std::endl;
}
}  // namespace chessgen
