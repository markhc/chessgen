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

#include "chessgen/game_state.hpp"

namespace chessgen
{
// -------------------------------------------------------------------------------------------------
void GameState::clearEnPassant()
{
  mEnPassant.clear();
}
// -------------------------------------------------------------------------------------------------
void GameState::updateNonPieceBitboards()
{
  mAllPieces[Color::White] =
      mPieces[Color::White][Piece::Pawn] | mPieces[Color::White][Piece::Rook] |
      mPieces[Color::White][Piece::Knight] | mPieces[Color::White][Piece::Bishop] |
      mPieces[Color::White][Piece::Queen] | mPieces[Color::White][Piece::King];

  mAllPieces[Color::Black] =
      mPieces[Color::Black][Piece::Pawn] | mPieces[Color::Black][Piece::Rook] |
      mPieces[Color::Black][Piece::Knight] | mPieces[Color::Black][Piece::Bishop] |
      mPieces[Color::Black][Piece::Queen] | mPieces[Color::Black][Piece::King];

  mOccupied = mAllPieces[Color::White] | mAllPieces[Color::Black];
}
// -------------------------------------------------------------------------------------------------
}  // namespace chessgen
