#include "cppgen/move.hpp"
#include "cppgen/helpers.hpp"

namespace cppgen
{
// -------------------------------------------------------------------------------------------------
Move::Move() {}
// -------------------------------------------------------------------------------------------------
Move::Move(CastleSide side) : mType(Castling), mCastleSide(side) {}
// -------------------------------------------------------------------------------------------------
Move::Move(Square from, Square to, Type type) : mType(type), mFromSquare(from), mToSquare(to) {}
// -------------------------------------------------------------------------------------------------
Move::Move(Square from, Square to, Piece promoted)
    : mType(Promotion), mFromSquare(from), mToSquare(to), mPromotedTo(promoted)
{
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
Move Move::makeCastling(CastleSide side) { return Move{side}; }
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
CastleSide Move::getCastleSide() const { return mCastleSide; }
// -------------------------------------------------------------------------------------------------
bool Move::isEnPassant() const { return mType == Type::EnPassant; }
// -------------------------------------------------------------------------------------------------
Piece Move::promotedTo() const { return mPromotedTo; }
// -------------------------------------------------------------------------------------------------
}  // namespace cppgen
