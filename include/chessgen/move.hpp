#pragma once

#include <string_view>

#include "exceptions.hpp"
#include "types.hpp"

namespace chessgen
{
class Move
{
  enum Type : short {
    Normal,
    Promotion,
    Castling,
    EnPassant,
  };
  Move(Square from, Square to, Type type);
  Move(Square from, Square to, Piece promoted);

public:
  Move();

  static Move makeQuietMove(Square from, Square to);
  static Move makeCaptureMove(Square from, Square to);
  static Move makePromotion(Square from, Square to, Piece promotedTo);
  static Move makeEnPassant(Square from, Square to);
  static Move makeCastling(Square from, Square to);

  Square fromSquare() const;
  Square toSquare() const;
  bool   isPromotion() const;
  bool   isCastling() const;
  bool   isEnPassant() const;
  Piece  promotedTo() const;

  static int notationToIndex(std::string_view notation)
  {
    return static_cast<int>(makeSquare(File(notation[0] - 'a'), Rank(notation[1] - '0')));
  }
  static std::string indexToNotation(int index) { return to_string(Square(index)); }

private:
  Type   mType{Type::Normal};
  Piece  mPromotedTo{};
  Square mFromSquare{};
  Square mToSquare{};
};
}  // namespace chessgen
