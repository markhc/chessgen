#include "cppgen/bitboard.hpp"
#include "cppgen/attacks.hpp"

#include <sstream>

namespace cppgen
{
Bitboard Bitboard::getLineBetween(Square a, Square b)
{
  return attacks::getLineBetween(a, b) &
         ((Bitboards::AllSquares << (makeIndex(a) + (makeIndex(a) < makeIndex(b)))) ^
          (Bitboards::AllSquares << (makeIndex(b) + !(makeIndex(a) < makeIndex(b)))));
}
std::string Bitboard::prettyPrint() const
{
  std::stringstream os;

  os << "  +-----------------+\n";
  for (Rank r = Rank::Rank8; r >= Rank::Rank1; --r) {
    os << static_cast<int>(r) + 1 << " | ";
    for (File f = File::FileA; f <= File::FileH; ++f) {
      os << ((*this & makeSquare(f, r)) ? "x " : ". ");
    }

    os << "|\n";
  }

  os << "  +-----------------+\n";
  os << "    A B C D E F G H\n";

  return os.str();
}
}  // namespace cppgen
