#include "chessgen/move.hpp"
#include "chessgen/helpers.hpp"

namespace chessgen
{
// -------------------------------------------------------------------------------------------------
Move::Move() {}
// -------------------------------------------------------------------------------------------------
Move::Move(Square from, Square to, Type type)
{
  mFromSquare = from;
  mToSquare   = to;
  mPromotedTo = Piece::None;
  mType       = type;
}
// -------------------------------------------------------------------------------------------------
Move::Move(Square from, Square to, Piece promoted)
{
  mFromSquare = from;
  mToSquare   = to;
  mPromotedTo = promoted;
  mType       = Promotion;
}
// -------------------------------------------------------------------------------------------------
Move Move::makeMove(Square from, Square to) { return Move{from, to, Normal}; }
// -------------------------------------------------------------------------------------------------
Move Move::makePromotion(Square from, Square to, Piece promotedTo)
{
  return Move{from, to, promotedTo};
}
// -------------------------------------------------------------------------------------------------
Move Move::makeEnPassant(Square from, Square to) { return Move{from, to, EnPassant}; }
// -------------------------------------------------------------------------------------------------
Move Move::makeCastling(Square from, Square to) { return Move{from, to, Castling}; }
// -------------------------------------------------------------------------------------------------
Move::Type Move::getType() const { return mType; }
// -------------------------------------------------------------------------------------------------
Square Move::fromSquare() const { return mFromSquare; }
// -------------------------------------------------------------------------------------------------
Square Move::toSquare() const { return mToSquare; }
// -------------------------------------------------------------------------------------------------
bool Move::isPromotion() const { return mType == Type::Promotion; }
// -------------------------------------------------------------------------------------------------
bool Move::isCastling() const { return mType == Type::Castling; }
// -------------------------------------------------------------------------------------------------
bool Move::isEnPassant() const { return mType == Type::EnPassant; }
// -------------------------------------------------------------------------------------------------
Piece Move::promotedTo() const { return mPromotedTo; }
// -------------------------------------------------------------------------------------------------
}  // namespace chessgen
