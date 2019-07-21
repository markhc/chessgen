#include "chessgen/movegen.hpp"
#include "chessgen/attacks.hpp"
#include "chessgen/board.hpp"
#include "chessgen/intrinsics.hpp"

#include <iostream>
#include <type_traits>

namespace chessgen
{
namespace movegen
{
template <Color Us>
void generateDiscoveredChecks(Board const& board, std::vector<Move>& moves);
template <Color Us, MoveType Type>
void generateAll(Board const& board, Bitboard target, std::vector<Move>& moves);

/**
 * General implementation. Handles Cpatures, NonEvasions and Quiet move generation.
 * Other move types are handled by their respective specializations below
 */
template <MoveType Type>
auto generateMoves(Board const& board) -> std::vector<Move>
{
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

template <>
auto generateMoves<MoveType::QuietChecks>(Board const& board) -> std::vector<Move>
{
  auto       moves = std::vector<Move>{};
  auto const us    = board.getActivePlayer();

  if (us == Color::White) {
    generateDiscoveredChecks<Color::White>(board, moves);
    generateAll<Color::White, MoveType::QuietChecks>(board, board.getUnoccupied(), moves);
  } else {
    generateDiscoveredChecks<Color::Black>(board, moves);
    generateAll<Color::Black, MoveType::QuietChecks>(board, board.getUnoccupied(), moves);
  }

  return moves;
}

template <>
auto generateMoves<MoveType::Evasions>(Board const& board) -> std::vector<Move>;
template <>
auto generateMoves<MoveType::Legal>(Board const& board) -> std::vector<Move>;

template <Color Us, MoveType Type>
void generateAll(Board const& board, Bitboard target, std::vector<Move>& moves)
{
}
template <Color Us>
void generateDiscoveredChecks(Board const& board, std::vector<Move>& moves)
{
  constexpr auto Them = ~Us;

  // Get all of our pieces that are blocking the attacks to the enemy king
  auto discoveredChecks = board.getKingBlockers(Them) & board.getAllPieces(Us);

  while (discoveredChecks) {
    auto const fromSquare = squareFromIndex(pop_lsb(discoveredChecks));
    auto const piece      = board.getPieceOn(fromSquare);

    // Handled in the special generatePawnMoves function
    if (piece == Piece::Pawn) continue;

    // Gets the possible squares this piece can move to without capturing
    auto b = board.getPossibleMoves(piece, Us, fromSquare) & board.getUnoccupied();

    // If the piece to move is our king, move it somewhere it won't keep blocking us
    // Other pieces (except pawns) will always move somewhere they won't block
    // so there is no need to check for them
    if (piece == Piece::King) {
      auto const kingSquare = bitscan_forward(board.getPieces(Them, Piece::King));

      b &= ~attacks::getSlidingAttacks(Piece::Queen, squareFromIndex(kingSquare), Bitboard{});
    }

    while (b) {
      moves.emplace_back(Move::makeQuietMove(fromSquare, squareFromIndex(pop_lsb(b))));
    }
  }
}
}  // namespace movegen
}  // namespace chessgen
