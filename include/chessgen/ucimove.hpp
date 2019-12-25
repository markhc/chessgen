#pragma once

#include "types.hpp"

namespace chessgen
{
class UCIMove
{
public:
  static constexpr struct EnPassant_t{} EnPassant{};

  UCIMove() = default;
  UCIMove(Square from, Square to) : mFromSquare(from), mToSquare(to)
  {
  }
  UCIMove(Square from, Square to, UCIMove::EnPassant_t)
      : mFromSquare(from), mToSquare(to), mEnPassant(true)
  {
  }
  UCIMove(Square from, Square to, Piece promotedTo)
      : mFromSquare(from), mToSquare(to), mPromotedTo(promotedTo)
  {
  }
  UCIMove(CastleSide side) : mCastleSide(side)
  {
  }

  Square fromSquare() const
  {
    return mFromSquare;
  }
  Square toSquare() const
  {
    return mToSquare;
  }
  bool isEnPassant() const
  {
    return mEnPassant;
  }
  bool isPromotion() const
  {
    return mPromotedTo != Piece::None;
  }
  Piece promotedTo() const
  {
    return mPromotedTo;
  }
  bool isCastling() const
  {
    return mCastleSide != CastleSide::None;
  }
  CastleSide getCastleSide() const
  {
    return mCastleSide;
  }

private:
  Square     mFromSquare{Square::None};
  Square     mToSquare{Square::None};
  CastleSide mCastleSide{CastleSide::None};
  Piece      mPromotedTo{Piece::None};
  bool       mEnPassant{false};
};
}  // namespace chessgen
