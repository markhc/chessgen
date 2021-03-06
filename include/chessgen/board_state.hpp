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
#include <string_view>

#include "bitboard.hpp"
#include "types.hpp"
#include "ucimove.hpp"

namespace chessgen
{
struct PieceInfo {
  Piece type;
  Color color;
};

class BoardState
{
  friend class Board;

public:
  static BoardState fromFen(std::string_view view, ChessVariant variant);

  std::string getFen() const;
  std::string getSanForMove(UCIMove const& uci) const;
  int         getHalfMoves() const;
  int         getFullMove() const;
  Color       getActivePlayer() const;
  bool        isInCheck() const;
  bool        canShortCastle(Color color) const;
  bool        canLongCastle(Color color) const;
  CastleSide  getCastlingRights(Color color) const;
  Bitboard    getPieces(Piece type) const;
  Bitboard    getPieces(Color color, Piece type) const;
  Bitboard    getAllPieces(Color color) const;
  Bitboard    getOccupied() const;
  Bitboard    getUnoccupied() const;
  Bitboard    getEnPassant() const;
  Bitboard    getPossibleMoves(Piece type, Color color, Square fromSquare) const;
  Bitboard    getKingBlockers(Color color) const;
  PieceInfo   getPieceOn(Square sq) const;
  Color       getColorOfPieceOn(Square sq) const;
  bool        isSquareEmpty(Square sq) const;
  Bitboard    getCheckSquares(Color color, Piece piece) const;
  Bitboard    getCheckers() const;
  Square      getKingSquare(Color color) const;
  Square      getCastlingRookSquare(Color color, CastleSide side) const;
  Square      getEnPassantSquare() const;
  bool        isSquareUnderAttack(Color enemy, Square square) const;
  bool        isMoveCheck(UCIMove const& move) const;
  bool        isMoveMate(UCIMove const& move) const;

private:
  void     clearEnPassant();
  void     updateNonPieceBitboards();
  void     addPiece(Piece type, Color color, Square square);
  void     removePiece(Piece type, Color color, Square square);
  void     movePiece(Piece type, Color color, Square from, Square to);
  bool     makeMove(UCIMove const& move);
  Bitboard getAttackers(Color color, Square square) const;
  Bitboard getWhitePawnAttacksForSquare(Square square) const;
  Bitboard getBlackPawnAttacksForSquare(Square square) const;
  Bitboard getKingAttacksForSquare(Square square, Color color) const;
  Bitboard getKnightAttacksForSquare(Square square, Color color) const;
  Bitboard getBishopAttacksForSquare(Square square, Color color) const;
  Bitboard getRookAttacksForSquare(Square square, Color color) const;
  Bitboard getQueenAttacksForSquare(Square square, Color color) const;
  
  Bitboard         mPieces[ColorCount][PieceCount]{};
  Bitboard         mAllPieces[ColorCount]{};
  Bitboard         mOccupied{};
  Bitboard         mEnPassant{};
  Color            mTurn{ColorWhite};
  int              mHalfMoves{0};
  int              mFullMove{1};
  CastleSide       mCastleRights[ColorCount]{};
};
}  // namespace chessgen
