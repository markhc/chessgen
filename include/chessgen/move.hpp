#pragma once

#include <string_view>

#include "exceptions.hpp"
#include "types.hpp"

namespace chessgen
{
enum class MoveType {
  Unknown,    ///< Invalid move
  PawnMove,   ///< A pawn moving forwards or capturing another piece, e.g: "e4", "g8=Q+"
  PieceMove,  ///< A piece (not a pawn) moving to another square, e.g: "Nf5",
              ///< "Rad1", "Qa1d2#"
  ShortCastle,
  LongCastle,
};

class Move
{
public:
  Move(std::string_view san);

  File   fromFile() const;
  Rank   fromRank() const;
  Square toSquare() const;
  Piece  pieceType() const;
  bool   isCapture() const;
  bool   isCheck() const;
  bool   isMate() const;
  bool   isPromotion() const;
  bool   isShortCastle() const;
  bool   isLongCastle() const;
  bool   isCastle() const;
  Piece  promotedTo() const;

  std::string toSan() const;

  static int notationToIndex(std::string_view notation)
  {
    return static_cast<int>(makeSquare(File(notation[0] - 'a'), Rank(notation[1] - '0')));
  }
  static std::string indexToNotation(int index) { return to_string(Square(index)); }

private:
  void parsePawnMove(std::string_view san);
  void parsePieceMove(std::string_view san);

  struct PawnMove {
    File   fromFile{};
    Square toSquare{};
    Piece  promotedTo{};
    bool   isPromotion{};
    bool   isCapture{};
  };

  struct PieceMove {
    Piece  piece{};
    File   fromFile{};
    Rank   fromRank{};
    Square toSquare{};
    bool   isCapture{};
  };

  union MoveInfo {
    MoveInfo();
    PawnMove  pawnMove;
    PieceMove pieceMove;
  };

  MoveType mType;
  MoveInfo mInfo;
  bool     mIsCheck{};
  bool     mIsMate{};
};
}  // namespace chessgen
