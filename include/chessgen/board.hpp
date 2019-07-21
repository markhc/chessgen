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
  /**
   * @brief Constructs a board in the default initial position
   */
  Board();

  /**
   * @brief Constructs a board in the given position
   */
  Board(std::string_view initialFen);

  /**
   * @brief Sets the board to the given X-FEN position
   *
   * @param fen A position using the X-FEN notation
   */
  void loadFen(std::string_view fen);

  /**
   * @brief Gets the X-FEN notation for the current board position
   */
  std::string getFen() const;

  /**
   * @brief Builds a string representation of the current board position
   *
   * @param useUnicodeChars Whether to use the unicode chess pieces instead
   *                        of letters for the pieces in the board
   *
   * @returns The current boars position in a string representation
   */
  std::string prettyPrint(bool useUnicodeChars = true);

  void makeMove(std::string_view move);
  void makeMove(Move move);
  Move undoMove();

  /**
   * @brief Gets the number of halfmoves since the last capture or pawn move
   *
   * The number of halfmoves is used to determine if a draw can be claimed
   * by the Fifty-move rule (https://en.wikipedia.org/wiki/Fifty-move_rule)
   *
   * @returns The current number of halfmoves since the last capture or pawn move
   */
  int getHalfMoves() const;

  /**
   * @brief Gets the fullmove number
   *
   * The fullmove number starts at 1 and is incremented after every black move
   *
   * @returns The current fullmove number
   */
  int getFullMove() const;

  /**
   * @brief Checks whether the board is in the initial chess position
   */
  bool isInitialPosition() const;

  /**
   * @brief Gets the current active player
   */
  Color getActivePlayer() const;

  /**
   * @brief Checks whether the given color is in check by the enemy
   */
  bool isInCheck(Color color) const;

  /**
   * @brief Checks whether the given color can castle king-side
   */
  bool canShortCastle(Color color) const;

  /**
   * @brief Checks whether the given color can castle queen-side
   */
  bool canLongCastle(Color color) const;

  /**
   * @brief Gets a bitflag enumeration with the castle rights for the given color
   */
  CastleSide getCastlingRights(Color color) const;

  /**
   * @brief Retrieves a bitboard with the position of the given piece regardless of color
   */
  Bitboard getPieces(Piece type) const;

  /**
   * @brief Retrieves a bitboard with the position of the pieces of the given type
   *        belonging to the player of the given color
   */
  Bitboard getPieces(Color color, Piece type) const;

  /**
   * @brief Retrieves a bitboard with the position of all the pieces of the given color
   */
  Bitboard getAllPieces(Color color) const;

  /**
   * @brief Retrieves a bitboard with all the occupied squares in the board
   */
  Bitboard getOccupied() const;

  /**
   * @brief Retrieves a bitboard with all the empty squares in the board
   */
  Bitboard getUnoccupied() const;

  /**
   * @brief Retrieves a bitboard with the enpassant square position.
   *        If there is no enpassant pawn, the value is 0
   */
  Bitboard getEnPassant() const;

  /**
   * @brief Retrives a bitboard with all the pseudo-legal moves for the given piece in the current board
   *
   * Determines all the squares the given piece type can move to from the source square
   * in the current board configuration. No consideration is done whether the moves are
   * illegal (would leave the king in check)
   *
   * @param type       The piece type
   * @param color      The piece color
   * @param fromSquare The source square
   *
   * @returns A bitboard of all the attacked squares
   */
  Bitboard getPossibleMoves(Piece type, Color color, Square fromSquare) const;

  /**
   * @brief Retrieves a bitboard with all the pieces blocking attacks to the king (possible
   * discovered checks or pins)
   *
   * @param color Color of the king
   *
   * @returns A bitboard with the positions of all the pieces blocking possible attacks to the king
   */
  Bitboard getKingBlockers(Color color) const;

  Piece getPieceOn(Square sq) const;

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
