#include <gtest/gtest.h>
#include <chessgen/board.hpp>
#include <chessgen/movegen.hpp>
#include <iostream>

TEST(MoveGenTests, PawnMoves)
{
  using namespace chessgen;
  using namespace chessgen::movegen;

  srand(123123);

  Board board{};

  while (true) {
    auto moves = generateMoves<MoveType::Legal>(board);

    if (moves.size() == 0) break;

    // pick a random move

    auto const i = random() % moves.size();

    std::cout << "Playing: " << to_string(moves[i].fromSquare()) << " "
              << to_string(moves[i].toSquare()) << std::endl;
    board.makeMove(moves[i]);

    std::cout << board.prettyPrint() << std::endl;
    std::cout << board.getFen() << std::endl;
    if (board.isInsufficientMaterial()) {
      std::cout << "Game is drawn by insufficient material\n";
      break;
    }
  }
  if (board.isInCheck()) {
    std::cout << ((board.getActivePlayer() == Color::White) ? "Black" : "White")
              << " wins by checkmate\n";
  } else {
    std::cout << "Game is drawn by stalemate\n";
  }
}
