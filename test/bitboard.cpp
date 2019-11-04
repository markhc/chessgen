#include <gtest/gtest.h>

#include <cppgen/bitboard.hpp>

using cppgen::Bitboard;

TEST(Bitboard, DefaultConstructor)
{
  ASSERT_NO_THROW({ Bitboard board; });
}

TEST(Bitboard, DefaultIsZero)
{
  ASSERT_EQ(Bitboard{}.getBits(), 0LL);
}

TEST(Bitboard, ValueConstruction)
{
  ASSERT_EQ(Bitboard{1ULL << 00}.getBits(), 1ULL << 00);
  ASSERT_EQ(Bitboard{1ULL << 15}.getBits(), 1ULL << 15);
  ASSERT_EQ(Bitboard{1ULL << 31}.getBits(), 1ULL << 31);
  ASSERT_EQ(Bitboard{1ULL << 45}.getBits(), 1ULL << 45);
  ASSERT_EQ(Bitboard{1ULL << 63}.getBits(), 1ULL << 63);
}

TEST(Bitboard, LeftShifting)
{
  ASSERT_EQ(Bitboard{1ULL} << 00, Bitboard{1ULL << 00});
  ASSERT_EQ(Bitboard{1ULL} << 15, Bitboard{1ULL << 15});
  ASSERT_EQ(Bitboard{1ULL} << 31, Bitboard{1ULL << 31});
  ASSERT_EQ(Bitboard{1ULL} << 45, Bitboard{1ULL << 45});
  ASSERT_EQ(Bitboard{1ULL} << 63, Bitboard{1ULL << 63});
}

TEST(Bitboard, RightShifting)
{
  auto constexpr max = std::numeric_limits<std::uint64_t>::max();

  ASSERT_EQ(Bitboard{max} >> 00, Bitboard{max >> 00});
  ASSERT_EQ(Bitboard{max} >> 15, Bitboard{max >> 15});
  ASSERT_EQ(Bitboard{max} >> 31, Bitboard{max >> 31});
  ASSERT_EQ(Bitboard{max} >> 45, Bitboard{max >> 45});
  ASSERT_EQ(Bitboard{max} >> 63, Bitboard{max >> 63});
}

TEST(Bitboard, SettingAndClearingBits)
{
  Bitboard board;

  board.setBit(0);
  ASSERT_EQ(board.getBits(), 1ULL);
  board.setBit(1);
  ASSERT_EQ(board.getBits(), 3LL);
  board.setBit(62);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);
  board.setBit(63);
  ASSERT_EQ(board.getBits(), 0xC000000000000003ULL);
  board.clearBit(63);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);

  board.clear();
  ASSERT_EQ(board.getBits(), 0ULL);

  board.setBit(cppgen::Square::A1);
  ASSERT_EQ(board.getBits(), 1ULL);
  board.setBit(cppgen::Square::B1);
  ASSERT_EQ(board.getBits(), 3LL);
  board.setBit(cppgen::Square::G8);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);
  board.setBit(cppgen::Square::H8);
  ASSERT_EQ(board.getBits(), 0xC000000000000003ULL);
  board.clearBit(cppgen::Square::H8);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);
}

TEST(Bitboard, Intrinsics)
{
  Bitboard board{cppgen::Bitboards::FileA};

  EXPECT_EQ(board.popCount(), 8);
  EXPECT_EQ(board.lsb(), 0);
  EXPECT_EQ(board.msb(), 56);

  EXPECT_EQ(board.popLsb(), 0);

  EXPECT_EQ(board.popCount(), 7);
  EXPECT_EQ(board.lsb(), 8);
  EXPECT_EQ(board.msb(), 56);
}

TEST(Bitboard, ShiftTowards)
{
  constexpr auto clearSquare = [](auto bitboard, auto square) {
    bitboard.clearBit(square);
    return bitboard;
  };

  Bitboard board{cppgen::Bitboards::FileA};
  
  EXPECT_EQ(board.shiftTowards(cppgen::Direction::East), cppgen::Bitboards::FileB);
  EXPECT_EQ(board.shiftTowards(cppgen::Direction::West), Bitboard{});
  EXPECT_EQ(board.shiftTowards(cppgen::Direction::North),
            clearSquare(cppgen::Bitboards::FileA, cppgen::Square::A1));
  EXPECT_EQ(board.shiftTowards(cppgen::Direction::South),
            clearSquare(cppgen::Bitboards::FileA, cppgen::Square::A8));
}
