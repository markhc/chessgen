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

#include <map>
#include <string>
#include <string_view>

#include "types.hpp"

namespace chessgen
{
class SANMove
{
public:
  SANMove() = default;

  static SANMove parse(std::string_view movetext);
  
  Piece  piece() const { return mPiece; }
  Square toSquare() const { return mToSquare; }
  File   fromFile() const { return mFromFile; }
  Rank   fromRank() const { return mFromRank; }

  bool isPromotion() const { return mPromoted != PieceNone;  }
  Piece promotedTo() const { return mPromoted; }

  bool       isCastling() const { return mCastling != CastleSide::None; }
  CastleSide getCastleSide() const { return mCastling; }


private:
  SANMove(File fromFile, Rank fromRank, Square toSquare, Piece promotedTo);
  SANMove(Piece piece, File fromFile, Rank fromRank, Square toSquare);
  SANMove(CastleSide castling);

  Piece      mPiece;
  File       mFromFile;
  Rank       mFromRank;
  Square     mToSquare;
  Piece      mPromoted;
  CastleSide mCastling;
};
}  // namespace chessgen
