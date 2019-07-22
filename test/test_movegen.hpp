#include <gtest/gtest.h>
#include <chessgen/board.hpp>
#include <chessgen/movegen.hpp>
#include <iostream>

TEST(MoveGenTests, PawnMoves)
{
  using namespace chessgen;
  using namespace chessgen::movegen;

  Board board{"7k/3q4/8/8/4p3/8/3P4/3K4 w - - 0 1"};

  std::cout << "Legal: \n";
  for (auto&& move : generateMoves<MoveType::Legal>(board)) {
    if (move.isPromotion()) {
      std::cout << to_string(move.fromSquare()) << " -> " << to_string(move.toSquare()) << " - "
                << to_string(move.promotedTo()) << std::endl;
    } else if (move.isEnPassant()) {
      std::cout << to_string(move.fromSquare()) << " -> " << to_string(move.toSquare()) << " EP\n";
    } else if (move.isCastling()) {
      std::cout << to_string(move.fromSquare()) << " -> " << to_string(move.toSquare())
                << " O-O O-O-O\n";
    } else {
      std::cout << to_string(move.fromSquare()) << " -> " << to_string(move.toSquare()) << std::endl;
    }
  }
}
