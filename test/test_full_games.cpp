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

#include <chessgen/board.hpp>
#include <string_view>
#include <vector>

using chessgen::Board;
using chessgen::UCIMove;

struct MoveRecord {
  MoveRecord(std::string n) : notation(std::move(n))
  {
  }
  MoveRecord(std::string n, bool capture) : notation(std::move(n)), isCapture(capture)
  {
  }
  MoveRecord(std::string n, bool capture, bool check)
      : notation(std::move(n)), isCapture(capture), isCheck(check)
  {
  }
  MoveRecord(std::string n, bool capture, bool check, bool mate)
      : notation(std::move(n)), isCapture(capture), isCheck(check), isMate(mate)
  {
  }
  MoveRecord(std::string n, bool capture, bool check, bool mate, bool castle)
      : notation(std::move(n)), isCapture(capture), isCheck(check), isMate(mate), isCastle(castle)
  {
  }
  MoveRecord(std::string n, bool capture, bool check, bool mate, bool castle, bool enpassant)
      : notation(std::move(n)),
        isCapture(capture),
        isCheck(check),
        isMate(mate),
        isCastle(castle),
        isEnPassant(enpassant)
  {
  }

  std::string notation;
  bool        isCapture{false};
  bool        isCheck{false};
  bool        isMate{false};
  bool        isCastle{false};
  bool        isEnPassant{false};
};

struct GameRecord {
  std::string             initialFen;
  bool                    isOnGoing;
  bool                    isMate;
  bool                    isDrawn;
  std::vector<MoveRecord> moves;
};

#define MOVE(X) MoveRecord(X)
#define MOVE_CHECK(X) MoveRecord(X, false, true)
#define CAPTURE(X) MoveRecord(X, true)
#define CAPTURE_CHECK(X) MoveRecord(X, true, true)
#define CASTLE(X) MoveRecord(X, false, false, false, true)

// clang-format off
GameRecord game1 = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                    true,
                    false,
                    false,
{
  MOVE("e4"),         MOVE("d6"),        MOVE("d4"),
  MOVE("Nf6"),        MOVE("Nc3"),       MOVE("g6"),
  MOVE("Be3"),        MOVE("Bg7"),       MOVE("Qd2"),
  MOVE("c6"),         MOVE("f3"),        MOVE("b5"),
  MOVE("Nge2"),       MOVE("Nbd7"),      MOVE("Bh6"),
  CAPTURE("Bxh6"),    CAPTURE("Qxh6"),   MOVE("Bb7"),
  MOVE("a3"),         MOVE("e5"),        CASTLE("O-O-O"),
  MOVE("Qe7"),        MOVE("Kb1"),       MOVE("a6"),
  MOVE("Nc1"),        CASTLE("O-O-O"),   MOVE("Nb3"),
  CAPTURE("exd4"),    CAPTURE("Rxd4"),   MOVE("c5"),
  MOVE("Rd1"),        MOVE("Nb6"),       MOVE("g3"),
  MOVE("Kb8"),        MOVE("Na5"),       MOVE("Ba8"),
  MOVE("Bh3"),        MOVE("d5"),        MOVE_CHECK("Qf4+"),
  MOVE("Ka7"),        MOVE("Rhe1"),      MOVE("d4"),
  MOVE("Nd5"),        CAPTURE("Nbxd5"),  CAPTURE("exd5"),
  MOVE("Qd6"),        CAPTURE("Rxd4"),   CAPTURE("cxd4"),
  MOVE_CHECK("Re7+"), MOVE("Kb6"),       CAPTURE_CHECK("Qxd4+"),
  CAPTURE("Kxa5"),    MOVE_CHECK("b4+"), MOVE("Ka4"),
  MOVE("Qc3"),        CAPTURE("Qxd5"),   MOVE("Ra7"),
  MOVE("Bb7"),        CAPTURE("Rxb7"),   MOVE("Qc4"),
  CAPTURE("Qxf6"),    CAPTURE("Kxa3"),   CAPTURE_CHECK("Qxa6+"),
  CAPTURE("Kxb4"),    MOVE_CHECK("c3+"), CAPTURE("Kxc3"),
  MOVE_CHECK("Qa1+"), MOVE("Kd2"),       MOVE_CHECK("Qb2+"),
  MOVE("Kd1"),        MOVE("Bf1"),       MOVE("Rd2"),
  MOVE("Rd7"),        CAPTURE("Rxd7"),   CAPTURE("Bxc4"),
  CAPTURE("bxc4"),    CAPTURE("Qxh8"),   MOVE("Rd3"),
  MOVE("Qa8"),        MOVE("c3"),        MOVE_CHECK("Qa4+"),
  MOVE("Ke1"),        MOVE("f4"),        MOVE("f5"),
  MOVE("Kc1"),        MOVE("Rd2"),       MOVE("Qa7"),
}};

GameRecord game2 = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                    true,
                    false,
                    false, 
{
  MOVE("d4"),            MOVE("e6"),            MOVE("Bf4"),           MOVE("d5"),            MOVE("e3"),
  MOVE("Nf6"),           MOVE("Nf3"),           MOVE("Bd6"),           MOVE("Bg3"),           MOVE("c6"),
  MOVE("Bd3"),           MOVE("Qc7"),           MOVE("Nbd2"),          MOVE("h5"),            CAPTURE("Bxd6"),
  CAPTURE("Qxd6"),       CASTLE("O-O"),         MOVE("h4"),            MOVE("h3"),            MOVE("Nbd7"),
  MOVE("Ng5"),           MOVE("e5"),            MOVE("c3"),            MOVE("e4"),            MOVE("Bc2"),
  MOVE("Nh5"),           MOVE("Qg4"),           MOVE("Ndf6"),          CAPTURE("Qxh4"),       MOVE("Rh6"),
  MOVE("Bd1"),           MOVE("Bf5"),           MOVE("Re1"),           MOVE("Bg6"),           MOVE("g4"),
  MOVE("Nf4"),           MOVE("Qg3"),           CAPTURE_CHECK("Nxh3"), CAPTURE("Nxh3"),       CAPTURE_CHECK("Qxg3"),
  CAPTURE("fxg3"),       CAPTURE("Rxh3"),       MOVE("Kg2"),           MOVE("Rh6"),           MOVE("g5"),
  MOVE("Rh7"),           CAPTURE("gxf6"),       CAPTURE("gxf6"),       MOVE("Bg4"),           MOVE("f5"),
  MOVE("Be2"),           CASTLE("O-O-O"),       MOVE("Rh1"),           MOVE("Rg7"),           MOVE("Rh2"),
  MOVE("Kc7"),           MOVE("Rah1"),          MOVE("a5"),            MOVE("Rh8"),           MOVE("Rd6"),
  MOVE("Ra8"),           MOVE("Kb6"),           MOVE("Nb3"),           MOVE("f6"),            CAPTURE("Nxa5"),
  MOVE("Bf7"),           MOVE("Nb3"),           MOVE("Rd7"),           MOVE("Nc5"),           MOVE("Re7"),
  MOVE("a4"),            MOVE("Be8"),           MOVE("b4"),            MOVE("Rg5"),           MOVE_CHECK("a5"),
  MOVE("Kc7"),           MOVE("a6"),            CAPTURE("bxa6"),       CAPTURE_CHECK("Nxa6"), MOVE("Kd6"),
  MOVE("Rh8"),           MOVE("Bd7"),           MOVE("Rhd8"),          MOVE("Reg7"),          MOVE("Nc5"),
  CAPTURE_CHECK("Rxg3"), MOVE("Kf2"),           MOVE_CHECK("Rg2"),     MOVE("Ke1"),           MOVE("R2g3"),
  MOVE("Kd2"),           MOVE("f4"),            CAPTURE("Nxd7"),       CAPTURE_CHECK("fxe3"), MOVE("Kc2"),
  CAPTURE("Rxd7"),       CAPTURE_CHECK("Rxd7"), CAPTURE("Kxd7"),       MOVE("b5"),            CAPTURE("cxb5"),
  MOVE_CHECK("Ra7"), MOVE("Kc6")
}};
// clang-format on

GameRecord const games[] = {game2};

TEST(Games, FullGames)
{
  auto runGameTest = [](GameRecord const& record) {
    Board board(record.initialFen);

    for (auto&& moveRecord : record.moves) {
      auto ucimove = board.sanToUci(moveRecord.notation);
      if (!moveRecord.isCastle) {
        ASSERT_TRUE(board.getPieceOn(ucimove.fromSquare()).type != chessgen::PieceNone);
        if (moveRecord.isEnPassant) {
          ASSERT_TRUE(board.getPieceOn(ucimove.toSquare()).type == chessgen::PieceNone);
        } else if (moveRecord.isCapture) {
          ASSERT_TRUE(board.getPieceOn(ucimove.toSquare()).type != chessgen::PieceNone);
        }
      }

      ASSERT_NO_THROW({
        ASSERT_TRUE(board.makeMove(ucimove)) << moveRecord.notation << std::endl
                                             << board.prettyPrint(false);
      });

      if (moveRecord.isCastle) {
        ASSERT_FALSE(board.canLongCastle(~board.getActivePlayer()));
        ASSERT_FALSE(board.canShortCastle(~board.getActivePlayer()));
      }

      if (moveRecord.isCheck) {
        ASSERT_TRUE(board.isInCheck());
      } else if (moveRecord.isMate) {
        ASSERT_TRUE(board.isOver());
      } else {
        ASSERT_FALSE(board.isInCheck());
      }
    }

    if (record.isOnGoing) {
      ASSERT_FALSE(board.isOver());
    } else {
      ASSERT_TRUE(board.isOver());
      if (record.isMate) {
        ASSERT_EQ(board.getGameOverReason(), chessgen::GameOverReason::Mate);
      } else if (record.isDrawn) {
        ASSERT_TRUE(board.getGameOverReason() != chessgen::GameOverReason::OnGoing &&
                    board.getGameOverReason() != chessgen::GameOverReason::Mate);
      }
    }
  };

  for (auto&& game : games) {
    runGameTest(game);
  }
}
