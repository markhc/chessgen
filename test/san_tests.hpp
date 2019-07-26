#include <gtest/gtest.h>
#include <cppgen/board.hpp>

#include "samples.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>

struct Game {
  int                      result;  // -1 Black won, 0 draw, 1 White won
  std::vector<std::string> moves;
};

Game pgn_moves_to_game(std::string_view pgn);
void play_full_game(Game game);

TEST(cppgen, StandardAlgebraicNotation)
{
  for (auto game : game_samples) {
    play_full_game(pgn_moves_to_game(game));
  }
}
// -------------------------------------------------------------------------------------------------
Game pgn_moves_to_game(std::string_view pgn)
{
  Game game;

  if (pgn.rfind("0-1") != std::string_view::npos) {
    game.result = -1;
    pgn         = pgn.substr(0, pgn.size() - 4);
  } else if (pgn.rfind("1-0") != std::string_view::npos) {
    game.result = 1;
    pgn         = pgn.substr(0, pgn.size() - 4);
  } else if (pgn.rfind("1/2-1/2") != std::string_view::npos) {
    game.result = 0;
    pgn         = pgn.substr(0, pgn.size() - 8);
  }

  auto trim_spaces = [](std::string_view str) {
    str.remove_prefix(std::min(str.find_first_not_of(" \n\t\r"), str.size()));
    auto suffix = str.find_last_not_of(" \n\t\r");
    if (suffix != str.npos) str.remove_suffix(std::max(str.size() - suffix - 1, std::size_t{0}));
    return str;
  };

  auto start = 0;
  while (start < pgn.size()) {
    if (!std::isdigit(pgn[start])) throw std::runtime_error("Something went wrong. Invalid PGN?");

    // skip "10. "
    while (pgn[start++] != '.')
      ;

    // find the next dot
    auto count = 1;
    while (pgn[start + count] != '.') count++;

    auto fullmove = trim_spaces(pgn.substr(start, count - 2));

    game.moves.emplace_back(trim_spaces(fullmove.substr(0, fullmove.find(' '))));
    if (fullmove.find(' ') != fullmove.npos)
      game.moves.emplace_back(trim_spaces(fullmove.substr(fullmove.find(' ') + 1)));

    start += count - 1;
  }

  return game;
}
// -------------------------------------------------------------------------------------------------
void play_full_game(Game game)
{
  using cppgen::Board;
  using cppgen::GameOverReason;

  Board board;

  ASSERT_TRUE(board.isInitialPosition());
  for (auto&& move : game.moves) {
    ASSERT_TRUE(board.makeMove(move)) << "Move: " + move << '\n';
    if (move.find('+') != std::string_view::npos) {
      EXPECT_TRUE(board.isInCheck());
    } else if (move.find('#') != std::string_view::npos) {
      EXPECT_EQ(board.getGameOverReason(), GameOverReason::Mate);
    }
  }

  if (game.result != 0)
    EXPECT_EQ(board.getActivePlayer(), game.result == -1 ? cppgen::White : cppgen::Black);
}
// -------------------------------------------------------------------------------------------------
