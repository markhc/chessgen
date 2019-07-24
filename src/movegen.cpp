#include "chessgen/movegen.hpp"
#include "chessgen/attacks.hpp"
#include "chessgen/board.hpp"

#include <algorithm>
#include <iostream>
#include <type_traits>

namespace chessgen
{
namespace movegen
{
template <Color Us, MoveType Type>
void generateAll(Board const& board, Bitboard target, std::vector<Move>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us>
void generateDiscoveredChecks(Board const& board, std::vector<Move>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us, Piece PieceType, MoveType Type>
void generatePieceMoves(Board const& board, Bitboard target, std::vector<Move>& moves);
// -------------------------------------------------------------------------------------------------
template <Color Us, MoveType Type>
void generatePawnMoves(Board const& board, Bitboard target, std::vector<Move>& moves);
// -------------------------------------------------------------------------------------------------
bool legalityCheck(Board const& board, Move const& move);
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool legalityCheck(Board const& board, Move const& move)
{
  auto const us   = board.getActivePlayer();
  auto const from = move.fromSquare();
  auto const to   = move.toSquare();
  auto const ksq  = board.getKingSquare(us);

  CG_ASSERT(ksq != Square::None);

  // En passant captures are a tricky special case. Because they are rather
  // uncommon, we do it simply by testing whether the king is attacked after
  // the move is made.
  if (move.isEnPassant()) {
    auto const capsq    = to - (us == Color::White ? Direction::North : Direction::South);
    auto const occupied = (board.getOccupied() ^ from ^ capsq) | to;

    CG_ASSERT(to == board.getEnPassantSquare());
    CG_ASSERT(!(board.getPieces(~us, Piece::Pawn) & capsq).isZero());
    CG_ASSERT(board.getPieceOn(to) == Piece::None);

    return !(attacks::getSlidingAttacks(Piece::Rook, ksq, occupied) &
             (board.getPieces(~us, Piece::Queen) | board.getPieces(~us, Piece::Rook))) &&
           !(attacks::getSlidingAttacks(Piece::Bishop, ksq, occupied) &
             (board.getPieces(~us, Piece::Queen) | board.getPieces(~us, Piece::Bishop)));
  }

  // If the moving piece is a king, check whether the destination square is
  // attacked by the opponent.
  if (ksq == from) {
    return !board.isSquareUnderAttack(~us, to);
  }

  // A non-king move is legal if and only if it is not pinned or it
  // is moving along the ray towards or away from the king.
  return !(board.getKingBlockers(us) & from) || (attacks::getLineBetween(from, to) & ksq);
}
// -------------------------------------------------------------------------------------------------
template <Color Us>
void generateDiscoveredChecks(Board const& board, std::vector<Move>& moves)
{
  constexpr auto Them = ~Us;

  CG_ASSERT(board.getKingSquare(Them) != Square::None);

  // Get all of our pieces that are blocking the attacks to the enemy king
  auto discoveredChecks = board.getKingBlockers(Them) & board.getAllPieces(Us);

  while (discoveredChecks) {
    auto const fromSquare = makeSquare(discoveredChecks.popLsb());
    auto const piece      = board.getPieceOn(fromSquare);

    // Handled in the special generatePawnMoves function
    if (piece == Piece::Pawn) continue;

    // Gets the possible squares this piece can move to without capturing
    auto b = board.getPossibleMoves(piece, Us, fromSquare) & board.getUnoccupied();

    // If the piece to move is our king, move it somewhere it won't keep blocking us
    // Other pieces (except pawns) will always move somewhere they won't block
    // so there is no need to check for them
    if (piece == Piece::King) {
      auto const kingSquare = board.getKingSquare(Them);

      b &= ~attacks::getSlidingAttacks(Piece::Queen, kingSquare, Bitboard{});
    }

    while (b) {
      moves.emplace_back(Move::makeMove(fromSquare, makeSquare(b.popLsb())));
    }
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, Piece PieceType, MoveType Type>
void generatePieceMoves(Board const& board, Bitboard target, std::vector<Move>& moves)
{
  CG_ASSERT(PieceType != Piece::King);

  if constexpr (PieceType == Piece::Pawn) {
    return generatePawnMoves<Us, Type>(board, target, moves);
  }

  auto squares = board.getPieces(Us, PieceType);

  while (squares) {
    auto const from            = makeSquare(squares.popLsb());
    auto       possibleSquares = board.getPossibleMoves(PieceType, Us, from) & target;

    if constexpr (Type == MoveType::QuietChecks) {
      possibleSquares &= board.getCheckSquares(Us, PieceType);
    }

    while (possibleSquares) {
      moves.emplace_back(Move::makeMove(from, makeSquare(possibleSquares.popLsb())));
    }
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, MoveType Type, Direction D>
void makePromotions(Board const& board, Square to, Square ksq, std::vector<Move>& moves)
{
  if constexpr (Type == MoveType::Captures || Type == MoveType::Evasions || Type == MoveType::NonEvasions)
    moves.emplace_back(Move::makePromotion(to - D, to, Piece::Queen));

  if constexpr (Type == MoveType::Quiets || Type == MoveType::Evasions ||
                Type == MoveType::NonEvasions) {
    moves.emplace_back(Move::makePromotion(to - D, to, Piece::Rook));
    moves.emplace_back(Move::makePromotion(to - D, to, Piece::Bishop));
    moves.emplace_back(Move::makePromotion(to - D, to, Piece::Knight));
  } else if (Type == MoveType::QuietChecks && (board.getPossibleMoves(Piece::Knight, Us, to) & ksq)) {
    moves.emplace_back(Move::makePromotion(to - D, to, Piece::Knight));
  }
}
// -------------------------------------------------------------------------------------------------
template <Color Us, MoveType Type>
void generatePawnMoves(Board const& board, Bitboard target, std::vector<Move>& moves)
{
  constexpr auto Them    = (Us == Color::White ? Color::Black : Color::White);
  constexpr auto Rank7BB = (Us == Color::White ? Bitboards::Rank7 : Bitboards::Rank2);
  constexpr auto Rank3BB = (Us == Color::White ? Bitboards::Rank3 : Bitboards::Rank6);
  constexpr auto Up      = (Us == Color::White ? Direction::North : Direction::South);
  constexpr auto UpRight = (Us == Color::White ? Direction::NorthEast : Direction::SouthWest);
  constexpr auto UpLeft  = (Us == Color::White ? Direction::NorthWest : Direction::SouthEast);

  auto pawnsOn7    = board.getPieces(Us, Piece::Pawn) & Rank7BB;
  auto pawnsNotOn7 = board.getPieces(Us, Piece::Pawn) & ~Rank7BB;

  auto const enemies = [&]() {
    if constexpr (Type == MoveType::Evasions)
      return board.getAllPieces(Them) & target;
    else if constexpr (Type == MoveType::Captures)
      return target;
    else
      return board.getAllPieces(Them);
  }();

  auto emptySquares = Bitboard{};

  if constexpr (Type != MoveType::Captures) {
    emptySquares =
        (Type == MoveType::Quiets || Type == MoveType::QuietChecks ? target : board.getUnoccupied());

    Bitboard singleMoves = pawnsNotOn7.shiftTowards(Up) & emptySquares;
    Bitboard doubleMoves = (singleMoves & Rank3BB).shiftTowards(Up) & emptySquares;

    if constexpr (Type == MoveType::Evasions)  // Consider only blocking squares
    {
      singleMoves &= target;
      doubleMoves &= target;
    }

    if constexpr (Type == MoveType::QuietChecks) {
      auto const ksq = board.getKingSquare(Them);

      singleMoves &= board.getPossibleMoves(Piece::Pawn, Them, ksq);
      doubleMoves &= board.getPossibleMoves(Piece::Pawn, Them, ksq);

      // Add pawn pushes which give discovered check. This is possible only
      // if the pawn is not on the same file as the enemy king, because we
      // don't generate captures. Note that a possible discovery check
      // promotion has been already generated amongst the captures.
      auto const dcCandidateQuiets = board.getKingBlockers(Them) & pawnsNotOn7;
      if (dcCandidateQuiets) {
        auto const file = makeIndex(getFile(ksq));
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
      moves.emplace_back(Move::makeMove(from, to));
    }

    while (doubleMoves) {
      auto const to   = makeSquare(doubleMoves.popLsb());
      auto const from = to - Up - Up;
      moves.emplace_back(Move::makeMove(from, to));
    }
  }

  if (pawnsOn7) {
    if constexpr (Type == MoveType::Captures) {
      emptySquares = board.getUnoccupied();
    }

    if constexpr (Type == MoveType::Evasions) {
      emptySquares &= target;
    }

    auto b1 = pawnsOn7.shiftTowards(UpRight) & enemies;
    auto b2 = pawnsOn7.shiftTowards(UpLeft) & enemies;
    auto b3 = pawnsOn7.shiftTowards(Up) & emptySquares;

    Square ksq = board.getKingSquare(Them);

    while (b1) {
      auto const sq = makeSquare(b1.popLsb());
      makePromotions<Us, Type, UpRight>(board, sq, ksq, moves);
    }

    while (b2) {
      auto const sq = makeSquare(b2.popLsb());
      makePromotions<Us, Type, UpLeft>(board, sq, ksq, moves);
    }

    while (b3) {
      auto const sq = makeSquare(b3.popLsb());
      makePromotions<Us, Type, Up>(board, sq, ksq, moves);
    }
  }

  if constexpr (Type == MoveType::Captures || Type == MoveType::Evasions ||
                Type == MoveType::NonEvasions) {
    Bitboard b1 = pawnsNotOn7.shiftTowards(UpRight) & enemies;
    Bitboard b2 = pawnsNotOn7.shiftTowards(UpLeft) & enemies;

    while (b1) {
      auto const to = makeSquare(b1.popLsb());
      moves.emplace_back(Move::makeMove(to - UpRight, to));
    }

    while (b2) {
      auto const to = makeSquare(b2.popLsb());
      moves.emplace_back(Move::makeMove(to - UpLeft, to));
    }

    if (board.getEnPassant()) {
      auto const ep = board.getEnPassantSquare();

      CG_ASSERT(getRank(ep) == (Us == Color::White ? Rank::Rank6 : Rank::Rank3));

      auto pawnSquare = ep - Up;

      // An en passant capture can be an evasion only if the checking piece
      // is the double pushed pawn and so is in the target. Otherwise this
      // is a discovery check and we are forced to do otherwise.
      if (Type == MoveType::Evasions && !(target & pawnSquare)) return;

      b1 = pawnsNotOn7 & board.getPossibleMoves(Piece::Pawn, Them, ep);

      // En passant squares are not recorded if there is no pawn in place to capture the passant
      // pawn so b1 should be always != 0
      CG_ASSERT(b1);

      while (b1) {
        moves.emplace_back(Move::makeEnPassant(makeSquare(b1.popLsb()), ep));
      }
    }
  }
}

/**
 * General implementation. Handles Captures, NonEvasions and Quiet move generation.
 * Other move types are handled by their respective specializations below
 */
template <MoveType Type>
auto generateMoves(Board const& board) -> std::vector<Move>
{
  CG_ASSERT(Type == MoveType::Captures || Type == MoveType::Quiets || Type == MoveType::NonEvasions);
  CG_ASSERT(!board.isInCheck());
  CG_ASSERT(!board.getCheckers());

  auto       moves = std::vector<Move>{};
  auto const us    = board.getActivePlayer();
  auto const them  = ~us;

  auto const target = [&] {
    // clang-format off
    if (Type == MoveType::Captures)    return  board.getAllPieces(them);
    if (Type == MoveType::Quiets)      return  board.getUnoccupied();
    if (Type == MoveType::NonEvasions) return ~board.getAllPieces(us);
    // clang-format on
    return Bitboard{0};
  }();

  if (us == Color::White)
    generateAll<Color::White, Type>(board, target, moves);
  else
    generateAll<Color::Black, Type>(board, target, moves);

  return moves;
}
template std::vector<Move> generateMoves<MoveType::Captures>(Board const& board);
template std::vector<Move> generateMoves<MoveType::Quiets>(Board const& board);
template std::vector<Move> generateMoves<MoveType::NonEvasions>(Board const& board);
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<MoveType::QuietChecks>(Board const& board) -> std::vector<Move>
{
  auto       moves = std::vector<Move>{};
  auto const us    = board.getActivePlayer();

  CG_ASSERT(!board.isInCheck());
  CG_ASSERT(!board.getCheckers());

  if (us == Color::White) {
    generateDiscoveredChecks<Color::White>(board, moves);
    generateAll<Color::White, MoveType::QuietChecks>(board, board.getUnoccupied(), moves);
  } else {
    generateDiscoveredChecks<Color::Black>(board, moves);
    generateAll<Color::Black, MoveType::QuietChecks>(board, board.getUnoccupied(), moves);
  }

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<MoveType::Evasions>(Board const& board) -> std::vector<Move>
{
  auto const us = board.getActivePlayer();

  CG_ASSERT(board.getKingSquare(us) != Square::None);
  CG_ASSERT(board.isInCheck());
  CG_ASSERT(board.getCheckers());

  auto moves         = std::vector<Move>{};
  auto ksq           = board.getKingSquare(us);
  auto sliderAttacks = Bitboard{};
  auto sliders       = board.getCheckers() &       //
                 ~board.getPieces(Piece::Pawn) &   //
                 ~board.getPieces(Piece::Knight);  //

  // Find all the squares attacked by slider checkers. We will remove them from
  // the king evasions in order to skip known illegal moves, which avoids any
  // useless legality checks later on.
  while (sliders) {
    auto const checksq = makeSquare(sliders.popLsb());
    sliderAttacks |= attacks::getLineBetween(checksq, ksq) ^ checksq;
  }

  // Generate evasions for king, capture and non capture moves
  auto b = board.getPossibleMoves(Piece::King, us, ksq) & ~board.getAllPieces(us) & ~sliderAttacks;
  while (b) {
    moves.emplace_back(Move::makeMove(ksq, makeSquare(b.popLsb())));
  }

  auto const checkers = board.getCheckers();

  // Double check?
  if (checkers.moreThanOne()) return moves;

  // Generate blocking evasions or captures of the checking piece
  auto const checksq = makeSquare(checkers.bsf());
  auto const target  = Bitboard::getLineBetween(checksq, ksq) | checksq;

  if (us == Color::White)
    generateAll<Color::White, MoveType::Evasions>(board, target, moves);
  else
    generateAll<Color::Black, MoveType::Evasions>(board, target, moves);

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <>
auto generateMoves<MoveType::Legal>(Board const& board) -> std::vector<Move>
{
  auto const us           = board.getActivePlayer();
  auto const pinnedPieces = board.getKingBlockers(us) & board.getAllPieces(us);
  auto const ksq          = board.getKingSquare(us);

  CG_ASSERT(ksq != Square::None);

  auto moves = std::vector<Move>{};
  if (board.isInCheck())
    moves = generateMoves<MoveType::Evasions>(board);
  else
    moves = generateMoves<MoveType::NonEvasions>(board);

  auto newEnd = std::remove_if(moves.begin(), moves.end(), [&](Move const& move) {
    // There are 2 situations in which a move can be invalid:
    // - If there are pinned pieces, it cannot be moved in a way that places the king in check
    // - If we are moving the king, it must not be placed in check
    //
    // If we could detect possible en passant captures that leave the king in check as a pinned
    // piece that would be nice, but since we cannot, we also check for that specific scenario here
    //
    if (pinnedPieces || move.fromSquare() == ksq || move.isEnPassant())
      return !legalityCheck(board, move);

    return false;
  });

  moves.erase(newEnd, moves.end());

  return moves;
}
// -------------------------------------------------------------------------------------------------
template <Color Us, MoveType Type>
void generateAll(Board const& board, Bitboard target, std::vector<Move>& moves)
{
  generatePieceMoves<Us, Piece::Pawn, Type>(board, target, moves);
  generatePieceMoves<Us, Piece::Knight, Type>(board, target, moves);
  generatePieceMoves<Us, Piece::Bishop, Type>(board, target, moves);
  generatePieceMoves<Us, Piece::Rook, Type>(board, target, moves);
  generatePieceMoves<Us, Piece::Queen, Type>(board, target, moves);

  if constexpr (Type != MoveType::QuietChecks && Type != MoveType::Evasions) {
    auto ksq = board.getKingSquare(Us);
    auto b   = board.getPossibleMoves(Piece::King, Us, ksq) & target;
    while (b) {
      moves.emplace_back(Move::makeMove(ksq, makeSquare(b.popLsb())));
    }

    if constexpr (Type != MoveType::Captures) {
      if (board.canLongCastle(Us))
        moves.emplace_back(Move::makeCastling(ksq, board.getCastlingRook(Us, CastleSide::Queen)));

      if (board.canShortCastle(Us))
        moves.emplace_back(Move::makeCastling(ksq, board.getCastlingRook(Us, CastleSide::King)));
    }
  }
}
// -------------------------------------------------------------------------------------------------
}  // namespace movegen
}  // namespace chessgen
