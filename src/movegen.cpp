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

#include "chessgen/movegen.hpp"

#include <algorithm>
#include <iostream>
#include <type_traits>

#include "chessgen/attacks.hpp"
#include "chessgen/board_state.hpp"

namespace chessgen
{
template <Color Us, GenType Type>
void generateAll(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us>
void generateDiscoveredChecks(class BoardState const& state, std::vector<UCIMove>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us, Piece PieceType, GenType Type>
void generatePieceMoves(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us, GenType Type>
void generatePawnMoves(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves);
// -------------------------------------------------------------------------------------------------
bool legalityCheck(class BoardState const& state, UCIMove const& move);
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool legalityCheck(class BoardState const& state, UCIMove const& move)
{
  auto const us   = state.getActivePlayer();
  auto const from = move.fromSquare();
  auto const to   = move.toSquare();
  auto const ksq  = state.getKingSquare(us);

  CHESSGEN_ASSERT(ksq != Square::None);

  // En passant captures are a tricky special case. Because they are rather
  // uncommon, we do it simply by testing whether the king is attacked after
  // the move is made.
  if (move.isEnPassant()) {
    auto const capsq    = to - (us == ColorWhite ? Direction::North : Direction::South);
    auto const occupied = (state.getOccupied() ^ from ^ capsq) | to;

    CHESSGEN_ASSERT(to == state.getEnPassantSquare());
    CHESSGEN_ASSERT(!(state.getPieces(~us, PiecePawn) & capsq).isZero());
    CHESSGEN_ASSERT(state.getPieceOn(to).type == PieceNone);

    return !(attacks::getSlidingAttacks(PieceRook, ksq, occupied) &
             (state.getPieces(~us, PieceQueen) | state.getPieces(~us, PieceRook))) &&
           !(attacks::getSlidingAttacks(PieceBishop, ksq, occupied) &
             (state.getPieces(~us, PieceQueen) | state.getPieces(~us, PieceBishop)));
  }

  // Castling moves already already checked for legality
  if (move.isCastling()) {
    return true;
  }

  // If the moving piece is a king, check whether the destination square is
  // attacked by the opponent.
  if (ksq == from) {
    return !state.isSquareUnderAttack(~us, to);
  }

  // A non-king move is legal if and only if it is not pinned or it
  // is moving along the ray towards or away from the king.
  return !(state.getKingBlockers(us) & from) || (attacks::getLineBetween(from, to) & ksq);
}
// -------------------------------------------------------------------------------------------------
template <Color Us>
void generateDiscoveredChecks(class BoardState const& state, std::vector<UCIMove>& moves)
{
  constexpr auto Them = ~Us;

  CHESSGEN_ASSERT(state.getKingSquare(Them) != Square::None);

  // Get all of our pieces that are blocking the attacks to the enemy king
  auto discoveredChecks = state.getKingBlockers(Them) & state.getAllPieces(Us);

  while (discoveredChecks) {
    auto const fromSquare = makeSquare(discoveredChecks.popLsb());
    auto const piece      = state.getPieceOn(fromSquare).type;

    // Handled in the special generatePawnMoves function
    if (piece == PiecePawn) continue;

    // Gets the possible squares this piece can move to without capturing
    auto b = state.getPossibleMoves(piece, Us, fromSquare) & state.getUnoccupied();

    // If the piece to move is our king, move it somewhere it won't keep blocking us
    // Other pieces (except pawns) will always move somewhere they won't block
    // so there is no need to check for them
    if (piece == PieceKing) {
      auto const kingSquare = state.getKingSquare(Them);

      b &= ~attacks::getSlidingAttacks(PieceQueen, kingSquare, Bitboard{});
    }

    while (b) {
      moves.emplace_back(fromSquare, makeSquare(b.popLsb()));
    }
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, Piece PieceType, GenType Type>
void generatePieceMoves(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves)
{
  CHESSGEN_ASSERT(PieceType != PieceKing);

  if constexpr (PieceType == PiecePawn) {
    return generatePawnMoves<Us, Type>(state, target, moves);
  }

  auto squares = state.getPieces(Us, PieceType);

  while (squares) {
    auto const from            = makeSquare(squares.popLsb());
    auto       possibleSquares = state.getPossibleMoves(PieceType, Us, from) & target;

    if constexpr (Type == GenType::QuietChecks) {
      possibleSquares &= state.getCheckSquares(Us, PieceType);
    }

    while (possibleSquares) {
      moves.emplace_back(from, makeSquare(possibleSquares.popLsb()));
    }
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, GenType Type, Direction D>
void makePromotions([[maybe_unused]] BoardState const&     state,
                    [[maybe_unused]] Square                to,
                    [[maybe_unused]] Square                ksq,
                    [[maybe_unused]] std::vector<UCIMove>& moves)
{
  if constexpr (Type == GenType::Captures || Type == GenType::Evasions || Type == GenType::NonEvasions)
    moves.emplace_back(to - D, to, PieceQueen);

  if constexpr (Type == GenType::Quiets || Type == GenType::Evasions || Type == GenType::NonEvasions) {
    moves.emplace_back(to - D, to, PieceRook);
    moves.emplace_back(to - D, to, PieceBishop);
    moves.emplace_back(to - D, to, PieceKnight);
  } else if (Type == GenType::QuietChecks && (state.getPossibleMoves(PieceKnight, Us, to) & ksq)) {
    moves.emplace_back(to - D, to, PieceKnight);
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, GenType Type>
void generatePawnMoves(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves)
{
  // clang-format off
  [[maybe_unused]] constexpr auto Them    = (Us == ColorWhite ? ColorBlack : ColorWhite);
  [[maybe_unused]] constexpr auto Rank7BB = (Us == ColorWhite ? Bitboards::Rank7 : Bitboards::Rank2);
  [[maybe_unused]] constexpr auto Rank3BB = (Us == ColorWhite ? Bitboards::Rank3 : Bitboards::Rank6);
  [[maybe_unused]] constexpr auto Up      = (Us == ColorWhite ? Direction::North : Direction::South);
  [[maybe_unused]] constexpr auto UpRight = (Us == ColorWhite ? Direction::NorthEast : Direction::SouthWest);
  [[maybe_unused]] constexpr auto UpLeft  = (Us == ColorWhite ? Direction::NorthWest : Direction::SouthEast);
  // clang-format on

  auto pawnsOn7    = state.getPieces(Us, PiecePawn) & Rank7BB;
  auto pawnsNotOn7 = state.getPieces(Us, PiecePawn) & ~Rank7BB;

  auto const enemies = [&]() {
    if constexpr (Type == GenType::Evasions)
      return state.getAllPieces(Them) & target;
    else if constexpr (Type == GenType::Captures)
      return target;
    else
      return state.getAllPieces(Them);
  }();

  auto emptySquares = Bitboard{};

  if constexpr (Type != GenType::Captures) {
    emptySquares =
        (Type == GenType::Quiets || Type == GenType::QuietChecks ? target : state.getUnoccupied());

    Bitboard singleMoves = pawnsNotOn7.shiftTowards(Up) & emptySquares;
    Bitboard doubleMoves = (singleMoves & Rank3BB).shiftTowards(Up) & emptySquares;

    if constexpr (Type == GenType::Evasions)  // Consider only blocking squares
    {
      singleMoves &= target;
      doubleMoves &= target;
    }

    if constexpr (Type == GenType::QuietChecks) {
      auto const ksq = state.getKingSquare(Them);

      singleMoves &= state.getPossibleMoves(PiecePawn, Them, ksq);
      doubleMoves &= state.getPossibleMoves(PiecePawn, Them, ksq);

      // Add pawn pushes which give discovered check. This is possible only
      // if the pawn is not on the same file as the enemy king, because we
      // don't generate captures. Note that a possible discovery check
      // promotion has been already generated amongst the captures.
      auto const dcCandidateQuiets = state.getKingBlockers(Them) & pawnsNotOn7;
      if (dcCandidateQuiets) {
        auto const file = int(getFile(ksq));
        auto const dc1 =
            dcCandidateQuiets.shiftTowards(Up) & emptySquares & ~(Bitboards::FileA << file);
        auto const dc2 = (dc1 & Rank3BB).shiftTowards(Up) & emptySquares;

        singleMoves |= dc1;
        doubleMoves |= dc2;
      }
    }

    while (singleMoves) {
      auto const to   = makeSquare(singleMoves.popLsb());
      auto const from = to - Up;
      moves.emplace_back(from, to);
    }

    while (doubleMoves) {
      auto const to   = makeSquare(doubleMoves.popLsb());
      auto const from = to - Up - Up;
      moves.emplace_back(from, to);
    }
  }

  if (pawnsOn7) {
    if constexpr (Type == GenType::Captures) {
      emptySquares = state.getUnoccupied();
    }

    if constexpr (Type == GenType::Evasions) {
      emptySquares &= target;
    }

    auto b1 = pawnsOn7.shiftTowards(UpRight) & enemies;
    auto b2 = pawnsOn7.shiftTowards(UpLeft) & enemies;
    auto b3 = pawnsOn7.shiftTowards(Up) & emptySquares;

    Square ksq = state.getKingSquare(Them);

    while (b1) {
      auto const sq = makeSquare(b1.popLsb());
      makePromotions<Us, Type, UpRight>(state, sq, ksq, moves);
    }

    while (b2) {
      auto const sq = makeSquare(b2.popLsb());
      makePromotions<Us, Type, UpLeft>(state, sq, ksq, moves);
    }

    while (b3) {
      auto const sq = makeSquare(b3.popLsb());
      makePromotions<Us, Type, Up>(state, sq, ksq, moves);
    }
  }

  if constexpr (Type == GenType::Captures || Type == GenType::Evasions || Type == GenType::NonEvasions) {
    Bitboard b1 = pawnsNotOn7.shiftTowards(UpRight) & enemies;
    Bitboard b2 = pawnsNotOn7.shiftTowards(UpLeft) & enemies;

    while (b1) {
      auto const to = makeSquare(b1.popLsb());
      moves.emplace_back(to - UpRight, to);
    }

    while (b2) {
      auto const to = makeSquare(b2.popLsb());
      moves.emplace_back(to - UpLeft, to);
    }

    if (state.getEnPassant()) {
      auto const ep = state.getEnPassantSquare();

      CHESSGEN_ASSERT(getRank(ep) == (Us == ColorWhite ? Rank::Rank6 : Rank::Rank3));

      auto pawnSquare = ep - Up;

      // An en passant capture can be an evasion only if the checking piece
      // is the double pushed pawn and so is in the target. Otherwise this
      // is a discovery check and we are forced to do otherwise.
      if (Type == GenType::Evasions && !(target & pawnSquare)) return;

      b1 = pawnsNotOn7 & state.getPossibleMoves(PiecePawn, Them, ep);

      // En passant squares are not recorded if there is no pawn in place to capture the passant
      // pawn so b1 should be always != 0
      CHESSGEN_ASSERT(b1);

      while (b1) {
        moves.emplace_back(makeSquare(b1.popLsb()), ep, UCIMove::EnPassant);
      }
    }
  }
}

/**
 * General implementation. Handles Captures, NonEvasions and Quiet move generation.
 * Other move types are handled by their respective specializations below
 */
template <GenType Type>
auto generateMoves(class BoardState const& state) -> std::vector<UCIMove>
{
  CHESSGEN_ASSERT(Type == GenType::Captures || Type == GenType::Quiets || Type == GenType::NonEvasions);
  CHESSGEN_ASSERT(!state.isInCheck());
  CHESSGEN_ASSERT(!state.getCheckers());

  auto       moves = std::vector<UCIMove>{};
  auto const us    = state.getActivePlayer();
  auto const them  = ~us;

  auto const target = [&] {
    // clang-format off
    if (Type == GenType::Captures)    return  state.getAllPieces(them);
    if (Type == GenType::Quiets)      return  state.getUnoccupied();
    if (Type == GenType::NonEvasions) return ~state.getAllPieces(us);
    // clang-format on
    return Bitboard{0};
  }();

  if (us == ColorWhite)
    generateAll<ColorWhite, Type>(state, target, moves);
  else
    generateAll<ColorBlack, Type>(state, target, moves);

  return moves;
}
template std::vector<UCIMove> generateMoves<GenType::Captures>(class BoardState const& state);
template std::vector<UCIMove> generateMoves<GenType::Quiets>(class BoardState const& state);
template std::vector<UCIMove> generateMoves<GenType::NonEvasions>(class BoardState const& state);
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<GenType::QuietChecks>(class BoardState const& state) -> std::vector<UCIMove>
{
  auto       moves = std::vector<UCIMove>{};
  auto const us    = state.getActivePlayer();

  CHESSGEN_ASSERT(!state.isInCheck());
  CHESSGEN_ASSERT(!state.getCheckers());

  if (us == ColorWhite) {
    generateDiscoveredChecks<ColorWhite>(state, moves);
    generateAll<ColorWhite, GenType::QuietChecks>(state, state.getUnoccupied(), moves);
  } else {
    generateDiscoveredChecks<ColorBlack>(state, moves);
    generateAll<ColorBlack, GenType::QuietChecks>(state, state.getUnoccupied(), moves);
  }

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<GenType::Evasions>(class BoardState const& state) -> std::vector<UCIMove>
{
  auto const us = state.getActivePlayer();

  CHESSGEN_ASSERT(state.getKingSquare(us) != Square::None);
  CHESSGEN_ASSERT(state.isInCheck());
  CHESSGEN_ASSERT(state.getCheckers());

  auto moves         = std::vector<UCIMove>{};
  auto ksq           = state.getKingSquare(us);
  auto sliderAttacks = Bitboard{};
  auto sliders       = state.getCheckers() &       //
                 ~state.getPieces(PiecePawn) &   //
                 ~state.getPieces(PieceKnight);  //

  // Find all the squares attacked by slider checkers. We will remove them from
  // the king evasions in order to skip known illegal moves, which avoids any
  // useless legality checks later on.
  while (sliders) {
    auto const checksq = makeSquare(sliders.popLsb());
    sliderAttacks |= attacks::getLineBetween(checksq, ksq) ^ checksq;
  }

  // Generate evasions for king, capture and non capture moves
  auto b = state.getPossibleMoves(PieceKing, us, ksq) & ~state.getAllPieces(us) & ~sliderAttacks;
  while (b) {
    moves.emplace_back(ksq, makeSquare(b.popLsb()));
  }

  auto const checkers = state.getCheckers();

  // Double check?
  if (checkers.moreThanOne()) return moves;

  // Generate blocking evasions or captures of the checking piece
  auto const checksq = makeSquare(checkers.lsb());
  auto const target  = Bitboard::getLineBetween(checksq, ksq) | checksq;

  if (us == ColorWhite)
    generateAll<ColorWhite, GenType::Evasions>(state, target, moves);
  else
    generateAll<ColorBlack, GenType::Evasions>(state, target, moves);

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<GenType::Legal>(class BoardState const& state) -> std::vector<UCIMove>
{
  auto const us           = state.getActivePlayer();
  auto const pinnedPieces = state.getKingBlockers(us) & state.getAllPieces(us);
  auto const ksq          = state.getKingSquare(us);

  CHESSGEN_ASSERT(ksq != Square::None);

  auto moves = std::vector<UCIMove>{};
  if (state.isInCheck())
    moves = generateMoves<GenType::Evasions>(state);
  else
    moves = generateMoves<GenType::NonEvasions>(state);

  auto newEnd = std::remove_if(moves.begin(), moves.end(), [&](UCIMove const& move) {
    // There are 2 situations in which a pseudo-legal move can be illegal:
    // - If there are pinned pieces, it cannot be moved in a way that places the king in check
    // - If we are moving the king, it must not be placed in check
    //
    // An extra scenario is also possible when there is an en passant capture. Ideally we would
    // detect that the capture would place the king in check and set the pawn as a pinned piece
    // but that is rather tricky. It's easier to just check for state legality after the move
    //
    if (pinnedPieces || move.fromSquare() == ksq || move.isEnPassant())
      return !legalityCheck(state, move);

    return false;
  });

  moves.erase(newEnd, moves.end());

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <Color Us, GenType Type>
void generateAll(class BoardState const& state, Bitboard target, std::vector<UCIMove>& moves)
{
  generatePieceMoves<Us, PiecePawn, Type>(state, target, moves);
  generatePieceMoves<Us, PieceKnight, Type>(state, target, moves);
  generatePieceMoves<Us, PieceBishop, Type>(state, target, moves);
  generatePieceMoves<Us, PieceRook, Type>(state, target, moves);
  generatePieceMoves<Us, PieceQueen, Type>(state, target, moves);

  if constexpr (Type != GenType::QuietChecks && Type != GenType::Evasions) {
    auto ksq = state.getKingSquare(Us);
    auto b   = state.getPossibleMoves(PieceKing, Us, ksq) & target;
    while (b) {
      moves.emplace_back(ksq, makeSquare(b.popLsb()));
    }

    if constexpr (Type != GenType::Captures) {
      if (state.canLongCastle(Us)) {
        moves.emplace_back(CastleSide::Queen);
      }

      if (state.canShortCastle(Us)) {
        moves.emplace_back(CastleSide::King);
      }
    }
  }
}
// -------------------------------------------------------------------------------------------------
}  // namespace chessgen
