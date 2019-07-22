#include <gtest/gtest.h>
#include <chessgen/board.hpp>
#include <chessgen/movegen.hpp>
#include <iostream>

TEST(MoveGenTests, PawnMoves)
{
  using namespace chessgen;
  using namespace chessgen::movegen;

  Board board{"4k3/3n1ppp/8/2pP4/1K6/5P2/5PPN/6NR w - c6 0 1"};

  std::cout << "NonEvasions: \n";
  for (auto&& move : generateMoves<MoveType::NonEvasions>(board)) {
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
  std::cout << "Evasions: \n";
  for (auto&& move : generateMoves<MoveType::Evasions>(board)) {
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
