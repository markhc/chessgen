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

#pragma once

#include <array>

#include "bitboard.hpp"
#include "types.hpp"

namespace chessgen
{
struct GameState {
  void clearEnPassant();
  void updateNonPieceBitboards();

  std::array<std::array<Bitboard, 6>, 2> mPieces{};
  std::array<Bitboard, 2>                mAllPieces{};
  Bitboard                               mOccupied{};
  Bitboard                               mEnPassant{};
  Color                                  mTurn{Color::White};
  int                                    mHalfMoves{0};
  int                                    mFullMove{1};
  std::array<CastleSide, 2>              mCastleRights{};
};
}  // namespace chessgen
