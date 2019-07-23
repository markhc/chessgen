#include <gtest/gtest.h>
#include <chessgen/board.hpp>
#include <iomanip>
#include <iostream>

#include <chrono>
#include <random>

void playGame(int id, int& nmoves, std::uint64_t& legalMoves)
{
  using namespace chessgen;

  std::mt19937 rng(id);

  Board board{};
  auto  movesThisGame = 0;
  while (true) {
    auto const& moves = board.getLegalMoves();
    legalMoves += moves.size();

    if (moves.size() == 0) break;

    // pick a random move

    auto const m = rng() % moves.size();

    // std::cout << "Playing: " << to_string(moves[m].fromSquare()) << " "
    //          << to_string(moves[m].toSquare()) << std::endl;
    board.makeMove(moves[m]);
    movesThisGame++;

    // std::cout << board.prettyPrint() << std::endl;
    // std::cout << board.getFen() << std::endl;
    if (board.isInsufficientMaterial()) {
      break;
    }
  }
  nmoves = movesThisGame;
}

void playGameDebug(int id)
{
  using namespace chessgen;

  std::mt19937 rng(id);

  Board board{};
  auto  movesThisGame = 0;
  while (true) {
    auto const& moves = board.getLegalMoves();

    if (moves.size() == 0) break;

    // pick a random move

    auto const m = rng() % moves.size();

    std::cout << "Playing: " << to_string(moves[m].fromSquare()) << " "
              << to_string(moves[m].toSquare()) << std::endl;
    board.makeMove(moves[m]);
    movesThisGame++;
    std::cout << board.prettyPrint() << std::endl;
    std::cout << board.getFen() << std::endl;
    if (board.isInsufficientMaterial()) {
      break;
    }
  }
  if (board.isInsufficientMaterial()) {
    std::cout << "Game #" << id << " is drawn by insufficient material\n";
  } else if (board.isInCheck()) {
    std::cout << "Game #" << id << " - "
              << ((board.getActivePlayer() == Color::White) ? "Black" : "White")
              << " wins by checkmate\n";
  } else {
    std::cout << "Game #" << id << " is drawn by stalemate\n";
  }
}

int main()
{
  using namespace chessgen;
  using namespace std::literals;

  auto i                   = int{};
  auto start               = std::chrono::high_resolution_clock::now();
  auto totalMoves          = std::uint64_t{};
  auto totalEvaluatedMoves = std::uint64_t{};
  auto mostMoves           = 0;
  auto mostMovesId         = 0;
  auto leastMoves          = 99999;
  auto leastMovesId        = 0;

  while (i < 10000) {
    int movesThisGame = 0;
    playGame(++i, movesThisGame, totalEvaluatedMoves);

    if (movesThisGame > mostMoves) {
      mostMoves   = movesThisGame;
      mostMovesId = i;
    } else if (movesThisGame < leastMoves) {
      leastMoves   = movesThisGame;
      leastMovesId = i;
    }
    totalMoves += movesThisGame;
  }
  auto end     = std::chrono::high_resolution_clock::now();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  std::cout << "Total play time:        " << seconds << " seconds\n";
  std::cout << "Total games played:     " << i << '\n';
  std::cout << "Average moves/game:     " << std::fixed << (totalMoves / (double)i) << '\n';
  std::cout << "Most moves in a game:   " << mostMoves << "(game #" << mostMovesId << ")\n";
  std::cout << "Least moves in a game:  " << leastMoves << "(game #" << leastMovesId << ")\n";
  std::cout << "Legal moves generated:  " << totalEvaluatedMoves << "\n";
  std::cout << "Moves generated/second: " << totalEvaluatedMoves / (double)(seconds) << "\n";

  playGameDebug(leastMovesId);

  //::testing::InitGoogleTest(&argc, argv);
  // return RUN_ALL_TESTS();
}
