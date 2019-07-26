#pragma once

#include <string_view>

#include "types.hpp"

namespace cppgen
{
class Move
{
public:
  enum Type : short {
    Normal,
    Promotion,
    Castling,
    EnPassant,
  };

private:
  Move(CastleSide side);
  Move(Square from, Square to, Type type);
  Move(Square from, Square to, Piece promotedTo);

public:
  Move();

  static Move makeMove(Square from, Square to);
  static Move makePromotion(Square from, Square to, Piece promotedTo);
  static Move makeEnPassant(Square from, Square to);
  static Move makeCastling(CastleSide side);

  Type       getType() const;
  Square     fromSquare() const;
  Square     toSquare() const;
  bool       isPromotion() const;
  bool       isCastling() const;
  CastleSide getCastleSide() const;
  bool       isEnPassant() const;
  Piece      promotedTo() const;

  static int notationToIndex(std::string_view notation)
  {
    return static_cast<int>(makeSquare(File(notation[0] - 'a'), Rank(notation[1] - '1')));
  }
  static std::string indexToNotation(int index) { return std::to_string(Square(index)); }

private:
  Type       mType{Type::Normal};
  Square     mFromSquare{Square::None};
  Square     mToSquare{Square::None};
  Piece      mPromotedTo{Piece::None};
  CastleSide mCastleSide{CastleSide::None};
};
}  // namespace cppgen
