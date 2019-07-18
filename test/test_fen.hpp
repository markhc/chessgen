#include <gtest/gtest.h>
#include <chessgen/board.hpp>

TEST(NotationTests, FEN)
{
  using namespace chessgen;

  Board board;

  ASSERT_TRUE(board.isInitialPosition());
  ASSERT_EQ(board.getActivePlayer(), Color::White);
  ASSERT_FALSE(board.canShortCastle(Color::White));
  ASSERT_FALSE(board.canShortCastle(Color::Black));
  ASSERT_FALSE(board.canLongCastle(Color::White));
  ASSERT_FALSE(board.canLongCastle(Color::Black));
  ASSERT_EQ(board.getCastlingRights(Color::White), (CastleSide::Queen | CastleSide::King));
  ASSERT_EQ(board.getEnPassant(), 0ULL);
  ASSERT_EQ(board.getOccupied(), board.getAllPieces(Color::White) | board.getAllPieces(Color::Black));
}
