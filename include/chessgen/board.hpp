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
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "bitboard.hpp"
#include "board_state.hpp"
#include "config.hpp"
#include "san.hpp"
#include "ucimove.hpp"

namespace chessgen
{
class Board
{
public:
  struct GameState {
    GameState(BoardState state, std::optional<UCIMove> move)
        : boardState(std::move(state)), movePlayed(std::move(move))
    {
    }
    BoardState             boardState;
    std::optional<UCIMove> movePlayed;
  };

  Board(Board const&);
  Board(Board&&) noexcept;
  Board& operator=(Board const&);
  Board& operator=(Board&&) noexcept;

public:
  /**
   * @brief Constructs a board in the default initial position
   */
  explicit Board(ChessVariant variant = ChessVariant::Standard);

  /**
   * @brief Constructs a board in the given position
   */
  explicit Board(std::string_view initialFen, ChessVariant variant = ChessVariant::Standard);

  /**
   * @brief Constructs a board from a saved state
   */
  explicit Board(BoardState const& state);

  void        loadFen(std::string_view fen, ChessVariant variant = ChessVariant::Standard);
  std::string getFen() const;
  std::string prettyPrint(bool useUnicodeChars = true) const;

  std::vector<UCIMove> const& getLegalMoves() const;
  std::vector<std::string>    getLegalMovesAsSAN() const;
  std::vector<UCIMove>        getLegalMovesForSquare(Square square) const;

  bool isValid(std::string_view move) const;
  bool isValid(Square from, Square to) const;
  bool isValid(CastleSide castle) const;
  bool isValid(SANMove const& move) const;
  bool isValid(UCIMove const& move) const;

  bool makeMove(std::string_view move);
  bool makeMove(Square from, Square to);
  bool makeMove(CastleSide side);
  bool makeMove(SANMove const& move);
  bool makeMove(UCIMove const& move);

  ChessVariant                  getVariant() const;
  UCIMove                       sanToUci(std::string_view move) const;
  std::string                   getSanForMove(UCIMove const& move) const;
  int                           getHalfMoves() const;
  int                           getFullMove() const;
  bool                          isInitialPosition() const;
  Color                         getActivePlayer() const;
  bool                          isOver() const;
  GameOverReason                getGameOverReason() const;
  bool                          isInCheck() const;
  BoardState const&             getState() const;
  std::vector<GameState> const& getGameHistory() const;
  bool                          canShortCastle(Color color) const;
  bool                          canLongCastle(Color color) const;
  Bitboard                      getPieces(Piece type) const;
  Bitboard                      getPieces(Color color, Piece type) const;
  PieceInfo                     getPieceOn(Square sq) const;
  bool                          isSquareEmpty(Square sq) const;
  Bitboard                      getCheckSquares(Color color, Piece piece) const;
  Bitboard                      getCheckers() const;
  Square                        getKingSquare(Color color) const;
  Square                        getEnPassantSquare() const;
  bool                          isSquareUnderAttack(Color enemy, Square square) const;

private:
  bool isInsufficientMaterial() const;
  bool isThreefold() const;
  bool isStalemate() const;
  bool isCheckmate() const;
  void gameOverCheck();

  template <typename Fn>
  auto findMoveIf(Fn f) const -> std::optional<UCIMove>
  {
    for (auto&& move : getLegalMoves()) {
      if (f(move)) {
        return move;
      }
    }
    return std::nullopt;
  }

  std::vector<GameState>              mStates;
  GameOverReason                      mReason{GameOverReason::OnGoing};
  ChessVariant                        mVariant{ChessVariant::Standard};
  mutable std::vector<UCIMove>        mLegalMoves;
  mutable std::atomic_bool            mBoardChanged{true};
  mutable std::mutex                  mMovesMutex{};
};
}  // namespace chessgen
