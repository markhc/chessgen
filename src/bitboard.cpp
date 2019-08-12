//
// Copyright (C) 2019-2019 markhc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

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
