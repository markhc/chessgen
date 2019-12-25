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

#include "chessgen/board.hpp"

#include <cctype>
#include <charconv>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "chessgen/attacks.hpp"
#include "chessgen/helpers.hpp"
#include "chessgen/movegen.hpp"

namespace chessgen
{
static std::string_view _initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static std::once_flag   _flag;

// -------------------------------------------------------------------------------------------------
Board::Board()
{
  std::call_once(_flag, [] { attacks::precomputeTables(); });
  loadFen(_initialFen);
}
// -------------------------------------------------------------------------------------------------
Board::Board(std::string_view initialFen)
{
  std::call_once(_flag, [] { attacks::precomputeTables(); });
  loadFen(initialFen);
}
// -------------------------------------------------------------------------------------------------
Board::Board(BoardState const& state)
    : mStates{}, mReason{GameOverReason::OnGoing}, mLegalMoves{}, mBoardChanged{true}
{
  mStates.push_back(GameState{state, std::nullopt});
  gameOverCheck();
}
// -------------------------------------------------------------------------------------------------
Board::Board(Board const& rhs)
    : mStates{rhs.mStates}, mReason{rhs.mReason}, mLegalMoves{}, mBoardChanged{true}
{
}
// -------------------------------------------------------------------------------------------------
Board::Board(Board&& rhs) noexcept
    : mStates{std::move(rhs.mStates)},
      mReason{std::move(rhs.mReason)},
      mLegalMoves{std::move(rhs.mLegalMoves)},
      mBoardChanged{rhs.mBoardChanged.load()}
{
}
// -------------------------------------------------------------------------------------------------
Board& Board::operator=(Board const& rhs)
{
  mStates       = rhs.mStates;
  mReason       = rhs.mReason;
  mLegalMoves   = rhs.mLegalMoves;
  mBoardChanged = rhs.mBoardChanged.load();

  return *this;
}
// -------------------------------------------------------------------------------------------------
Board& Board::operator=(Board&& rhs) noexcept
{
  mStates       = std::move(rhs.mStates);
  mReason       = std::move(rhs.mReason);
  mLegalMoves   = std::move(rhs.mLegalMoves);
  mBoardChanged = rhs.mBoardChanged.load();

  return *this;
}
// -------------------------------------------------------------------------------------------------
void Board::loadFen(std::string_view fen)
{
  {
    auto lock = std::unique_lock(mMovesMutex);
    mLegalMoves.clear();
  }
  mReason       = GameOverReason::OnGoing;
  mBoardChanged = true;
  mStates.clear();
  mStates.push_back(GameState{BoardState::fromFen(fen), std::nullopt});
}
// -------------------------------------------------------------------------------------------------
std::string Board::getFen() const
{
  return getState().toFen();
}
// -------------------------------------------------------------------------------------------------
std::string Board::prettyPrint(bool useUnicodeChars) const
{
  static std::string_view charPieces[2][6] = {
      {
          useUnicodeChars ? "\u2659" : "P",
          useUnicodeChars ? "\u2657" : "B",
          useUnicodeChars ? "\u2658" : "N",
          useUnicodeChars ? "\u2656" : "R",
          useUnicodeChars ? "\u2655" : "Q",
          useUnicodeChars ? "\u2654" : "K",
      },
      {
          useUnicodeChars ? "\u265F" : "p",
          useUnicodeChars ? "\u265D" : "b",
          useUnicodeChars ? "\u265E" : "n",
          useUnicodeChars ? "\u265C" : "r",
          useUnicodeChars ? "\u265B" : "q",
          useUnicodeChars ? "\u265A" : "k",
      },
  };

  std::stringstream ss;

  auto printIcon = [&](Square square) {
    auto const pieces = {
        Piece::Pawn,
        Piece::Bishop,
        Piece::Knight,
        Piece::Rook,
        Piece::Queen,
        Piece::King,
    };

    for (auto&& color : {Color::White, Color::Black}) {
      for (auto&& piece : pieces) {
        if (getState().getPieces(color, piece) & square) {
          ss << charPieces[int(color)][int(piece)] << ' ';
          return true;
        }
      }
    }
    return false;
  };

  ss << "  +-----------------+\n";

  // For each rank
  for (auto r = Rank::Rank8; r >= Rank::Rank1; --r) {
    // print the rank number
    ss << static_cast<int>(r) + 1 << " | ";
    // and each file
    for (auto f = File::FileA; f <= File::FileH; ++f) {
      if (!printIcon(makeSquare(f, r))) {
        ss << ". ";
      }
    }

    ss << "|\n";
  }

  ss << "  +-----------------+\n";
  ss << "    A B C D E F G H" << std::endl;
  return ss.str();
}
// -------------------------------------------------------------------------------------------------
std::vector<UCIMove> const& Board::getLegalMoves() const
{
  if (mBoardChanged) {
    auto lock = std::unique_lock{mMovesMutex};
    if (mBoardChanged) {
      mLegalMoves   = generateMoves<GenType::Legal>(getState());
      mBoardChanged = false;
    }
  }

  return mLegalMoves;
}
// -------------------------------------------------------------------------------------------------
std::vector<std::string> Board::getLegalMovesAsSAN() const
{
  auto result = std::vector<std::string>{};
  for (auto&& move : getLegalMoves()) {
    result.emplace_back(toSAN(move));
  }
  return result;
}
// -------------------------------------------------------------------------------------------------
std::vector<UCIMove> Board::getLegalMovesForSquare(Square square) const
{
  auto       result = std::vector<UCIMove>{};
  auto const ksq    = getState().getKingSquare(getActivePlayer());

  for (auto&& move : getLegalMoves()) {
    if (move.fromSquare() == square) {
      result.emplace_back(move);
    } else if (square == ksq && move.isCastling()) {
      result.emplace_back(move);
    }
  }
  return result;
}
// -------------------------------------------------------------------------------------------------
bool Board::isValid(std::string_view move) const
{
  return isValid(SANMove::parse(move));
}
// -------------------------------------------------------------------------------------------------
bool Board::isValid(Square from, Square to) const
{
  auto& state = getState();

  if (state.isSquareEmpty(from)) {
    return false;
  }
  if (state.getColorOfPieceOn(from) != getActivePlayer()) {
    return false;
  }
  if (state.getEnPassantSquare() == to) {
    return isValid(UCIMove{from, to, UCIMove::EnPassant});
  }
  if (state.getKingSquare(getActivePlayer()) == from) {
    if (int(to) == int(from) + 2) return isValid(CastleSide::King);
    if (int(to) == int(from) - 2) return isValid(CastleSide::Queen);
  }
  return isValid(UCIMove{from, to});
}
// -------------------------------------------------------------------------------------------------
bool Board::isValid(CastleSide castle) const
{
  auto& state = getState();
  return castle == CastleSide::King ? state.canShortCastle(getActivePlayer())
                                    : state.canLongCastle(getActivePlayer());
}
// -------------------------------------------------------------------------------------------------
bool Board::isValid(SANMove const& move) const
{
  if (move.isCastling()) {
    return isValid(move.getCastleSide());
  }

  auto& state = getState();

  for (auto&& m : getLegalMoves()) {
    if (state.getPieceOn(m.fromSquare()).type != move.piece()) {
      continue;
    }

    if (m.toSquare() == move.toSquare()) {
      if (move.fromFile() == File::None || move.fromFile() == getFile(m.fromSquare())) {
        if (move.fromRank() == Rank::None || move.fromRank() == getRank(m.fromSquare())) {
          if (move.isPromotion() == m.isPromotion()) {
            return true;
          }
        }
      }
    }
  }

  return false;
}
// -------------------------------------------------------------------------------------------------
bool Board::isValid(UCIMove const& move) const
{
  if (move.isCastling()) {
    return isValid(move.getCastleSide());
  }

  for (auto&& m : getLegalMoves()) {
    if (m.fromSquare() == move.fromSquare() && m.toSquare() == move.toSquare() &&
        (!m.isPromotion() || m.promotedTo() == move.promotedTo()))
      return true;
  }
  return false;
}
// -------------------------------------------------------------------------------------------------
UCIMove Board::sanToUci(std::string_view move) const
{
  auto const san = SANMove::parse(move);

  if (san.isCastling()) {
    return UCIMove{san.getCastleSide()};
  }

  auto& state = getState();

  for (auto&& m : getLegalMoves()) {
    if (state.getPieceOn(m.fromSquare()).type != san.piece()) {
      continue;
    }

    if (m.toSquare() == san.toSquare()) {
      if (san.fromFile() == File::None || san.fromFile() == getFile(m.fromSquare())) {
        if (san.fromRank() == Rank::None || san.fromRank() == getRank(m.fromSquare())) {
          if (san.isPromotion() == m.isPromotion()) {
            return m;
          }
        }
      }
    }
  }

  throw std::runtime_error("Invalid move");
}
// -------------------------------------------------------------------------------------------------
std::string Board::toSAN(UCIMove const& move) const
{
  return getState().toSAN(move);
}
// -------------------------------------------------------------------------------------------------
bool Board::makeMove(CastleSide side)
{
  return makeMove(UCIMove{side});
}
// -------------------------------------------------------------------------------------------------
bool Board::makeMove(SANMove const& move)
{
  if (move.isCastling()) {
    return makeMove(move.getCastleSide());
  }

  auto& state = getState();
  for (auto&& m : getLegalMoves()) {
    if (state.getPieceOn(m.fromSquare()).type != move.piece()) {
      continue;
    }

    if (m.toSquare() == move.toSquare()) {
      if (move.fromFile() == File::None || move.fromFile() == getFile(m.fromSquare())) {
        if (move.fromRank() == Rank::None || move.fromRank() == getRank(m.fromSquare())) {
          if (move.isPromotion() == m.isPromotion()) {
            return makeMove(m);
          }
        }
      }
    }
  }

  return false;
}
// -------------------------------------------------------------------------------------------------
bool Board::makeMove(UCIMove const& move)
{
  if (!isValid(move)) {
    return false;
  }

  mStates.back().movePlayed = move;

  auto state = getState();
  state.makeMove(move);

  mStates.push_back(GameState{state, std::nullopt});

  mBoardChanged = true;
  gameOverCheck();

  return true;
}
// -------------------------------------------------------------------------------------------------
bool Board::makeMove(std::string_view move)
{
  return makeMove(SANMove::parse(move));
}
// -------------------------------------------------------------------------------------------------
bool Board::makeMove(Square from, Square to)
{
  if (!isValid(from, to)) {
    return false;
  }

  auto& state = getState();

  if (state.getEnPassantSquare() == to) {
    return makeMove(UCIMove{from, to, UCIMove::EnPassant});
  }

  if (state.getKingSquare(getActivePlayer()) == from) {
    if (int(to) == int(from) + 2) return makeMove(UCIMove{CastleSide::King});
    if (int(to) == int(from) - 2) return makeMove(UCIMove{CastleSide::Queen});
  }
  return makeMove(UCIMove{from, to});
}
// -------------------------------------------------------------------------------------------------
int Board::getHalfMoves() const
{
  return getState().getHalfMoves();
}
// -------------------------------------------------------------------------------------------------
int Board::getFullMove() const
{
  return getState().getFullMove();
}
// -------------------------------------------------------------------------------------------------
bool Board::isInitialPosition() const
{
  return getFen() == _initialFen;
}
// -------------------------------------------------------------------------------------------------
Color Board::getActivePlayer() const
{
  return getState().getActivePlayer();
}
// -------------------------------------------------------------------------------------------------
bool Board::isOver() const
{
  return getGameOverReason() != GameOverReason::OnGoing;
}
// -------------------------------------------------------------------------------------------------
GameOverReason Board::getGameOverReason() const
{
  return mReason;
}
// -------------------------------------------------------------------------------------------------
bool Board::isInCheck() const
{
  auto& state = getState();

  auto const kingSquareIndex = state.getKingSquare(getActivePlayer());

  // no king?
  if (kingSquareIndex == Square::None) {
    return false;
  }

  return state.isSquareUnderAttack(~getActivePlayer(), kingSquareIndex);
}
// -------------------------------------------------------------------------------------------------
bool Board::isInsufficientMaterial() const
{
  auto const toMove = getActivePlayer();
  auto&      state  = getState();

  // Check if only kings are left
  if (state.getOccupied().popCount() == 2) {
    return true;
  }

  // Check King+Knight vs King scenarios
  if (!(state.getAllPieces(toMove) & ~state.getPieces(toMove, Piece::Knight))) {
    return true;
  }

  return false;
}
// -------------------------------------------------------------------------------------------------
bool Board::isThreefold() const
{
  // TODO: Support 3-fold repetition
  return false;
}
// -------------------------------------------------------------------------------------------------
bool Board::isStalemate() const
{
  return !isInCheck() && getLegalMoves().size() == 0;
}
// -------------------------------------------------------------------------------------------------
bool Board::isCheckmate() const
{
  return isInCheck() && getLegalMoves().size() == 0;
}
// -------------------------------------------------------------------------------------------------
void Board::gameOverCheck()
{
  if (getLegalMoves().size() == 0) {
    mReason = isInCheck() ? GameOverReason::Mate : GameOverReason::Stalemate;
  } else if (isInsufficientMaterial()) {
    mReason = GameOverReason::InsuffMaterial;
  } else if (isThreefold()) {
    mReason = GameOverReason::Threefold;
  }
}
// -------------------------------------------------------------------------------------------------
BoardState const& Board::getState() const
{
  return mStates.back().boardState;
}
// -------------------------------------------------------------------------------------------------
std::vector<Board::GameState> const& Board::getGameHistory() const
{
  return mStates;
}
// -------------------------------------------------------------------------------------------------
bool Board::canShortCastle(Color color) const
{
  return getState().canShortCastle(color);
}
// -------------------------------------------------------------------------------------------------
bool Board::canLongCastle(Color color) const
{
  return getState().canLongCastle(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getPieces(Piece type) const
{
  return getState().getPieces(type);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getPieces(Color color, Piece type) const
{
  return getState().getPieces(color, type);
}
// -------------------------------------------------------------------------------------------------
PieceInfo Board::getPieceOn(Square sq) const
{
  return getState().getPieceOn(sq);
}
// -------------------------------------------------------------------------------------------------
bool Board::isSquareEmpty(Square sq) const
{
  return getState().isSquareEmpty(sq);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getCheckSquares(Color color, Piece piece) const
{
  return getState().getCheckSquares(color, piece);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getCheckers() const
{
  return getState().getCheckers();
}
// -------------------------------------------------------------------------------------------------
Square Board::getKingSquare(Color color) const
{
  return getState().getKingSquare(color);
}
// -------------------------------------------------------------------------------------------------
Square Board::getEnPassantSquare() const
{
  return getState().getEnPassantSquare();
}
// -------------------------------------------------------------------------------------------------
bool Board::isSquareUnderAttack(Color enemy, Square square) const
{
  return getState().isSquareUnderAttack(enemy, square);
}
// -------------------------------------------------------------------------------------------------
}  // namespace chessgen
