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
#include <gtest/gtest.h>

#include <chessgen/bitboard.hpp>

using chessgen::Bitboard;

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

  board.setBit(chessgen::Square::A1);
  ASSERT_EQ(board.getBits(), 1ULL);
  board.setBit(chessgen::Square::B1);
  ASSERT_EQ(board.getBits(), 3LL);
  board.setBit(chessgen::Square::G8);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);
  board.setBit(chessgen::Square::H8);
  ASSERT_EQ(board.getBits(), 0xC000000000000003ULL);
  board.clearBit(chessgen::Square::H8);
  ASSERT_EQ(board.getBits(), 0x4000000000000003ULL);
}

TEST(Bitboard, Intrinsics)
{
  Bitboard board{chessgen::Bitboards::FileA};

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

  Bitboard board{chessgen::Bitboards::FileA};

  EXPECT_EQ(board.shiftTowards(chessgen::Direction::East), chessgen::Bitboards::FileB);
  EXPECT_EQ(board.shiftTowards(chessgen::Direction::West), Bitboard{});
  EXPECT_EQ(board.shiftTowards(chessgen::Direction::North),
            clearSquare(chessgen::Bitboards::FileA, chessgen::Square::A1));
  EXPECT_EQ(board.shiftTowards(chessgen::Direction::South),
            clearSquare(chessgen::Bitboards::FileA, chessgen::Square::A8));
}
