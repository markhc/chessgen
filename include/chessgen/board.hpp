#pragma once

#include <string>
#include <vector>

#include "bitboard.hpp"
#include "config.hpp"
#include "move.hpp"

namespace chessgen
{
enum class CastleSide {
  None  = (0),       ///< Players cannot castle
  King  = (1 << 0),  ///< White can castle on the king side
  Queen = (1 << 1),  ///< Black can castle on the king side
};

CHESSGEN_ENUMOPS(CastleSide)

class Board
{
public:
  Board();
  Board(std::string_view initialFen);

  void        loadFen(std::string_view fen);
  std::string getFen() const;
  std::string prettyPrint(bool useUnicodeChars = true);

  void makeMove(std::string_view move);
  void makeMove(Move move);
  Move undoMove();

  int        getHalfMoves() const;
  int        getFullMove() const;
  bool       isInitialPosition() const;
  Color      getActivePlayer() const;
  bool       isInCheck(Color color) const;
  bool       canShortCastle(Color color) const;
  bool       canLongCastle(Color color) const;
  CastleSide getCastlingRights(Color color) const;

  Bitboard getPieces(Color color, Piece type) const;
  Bitboard getAllPieces(Color color) const;
  Bitboard getOccupied() const;
  Bitboard getUnoccupied() const;
  Bitboard getEnPassant() const;

  /**
   * @brief Retrived all possible moves for the given piece
   *
   * Determines all the squares the given piece type can move to from the source square
   *
   * @param type       The piece type
   * @param color      The piece color
   * @param fromSquare The source square
   *
   * @returns A bitboard of all the attacked squares
   */
  Bitboard getPossibleMoves(Piece type, Color color, Square fromSquare) const;

private:
  /**
   * @brief Determines if the given square is under attack by the given color.
   *
   * @param color   The attacking side
   * @param square  The square index
   */
  bool isSquareUnderAttack(Color color, Square square) const;

  /**
   * @brief Adds a piece to the board and updates all related bitboards
   */
  void addPiece(Piece type, Color color, Square square);

  /**
   * @brief Removes a piece from the board and updates all related bitboards
   */
  void removePiece(Piece type, Color color, Square square);

  /**
   * @brief Clears all bitboard (removes all pieces from the board)
   */
  void clearBitboards();

  /**
   * @brief Updates all non-piece bitboards to reflect changes in the pieces bitboards
   */
  void updateBitboards();

  Bitboard getWhitePawnAttacksForSquare(Square square) const;
  Bitboard getBlackPawnAttacksForSquare(Square square) const;
  Bitboard getKingAttacksForSquare(Square square, Color color) const;
  Bitboard getKnightAttacksForSquare(Square square, Color color) const;
  Bitboard getBishopAttacksForSquare(Square square, Color color) const;
  Bitboard getRookAttacksForSquare(Square square, Color color) const;
  Bitboard getQueenAttacksForSquare(Square square, Color color) const;

  Bitboard   mPieces[2][6]{};
  Bitboard   mAllPieces[2]{};
  Bitboard   mOccupied;
  Bitboard   mEnPassant;
  Color      mTurn;
  int        mHalfMoves{};
  int        mFullMove{};
  CastleSide mCastleRights[2]{};
};
}  // namespace chessgen
