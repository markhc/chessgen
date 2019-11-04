# ChessGen 

A chess move generator and validator written in modern C++

This is still an early version, so bugs might (and probably do) exist. Please fill an issue if you find any.

## Build Status
| OS     | Windows                                                                                                                                 | Linux                                                                                                         |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| Master | [![Build status](https://ci.appveyor.com/api/projects/status/fmrgv06nwvoc2rv4?svg=true)](https://ci.appveyor.com/project/MarkHC/chessgen) | [![Build Status](https://travis-ci.org/MarkHC/chessgen.svg?branch=master)](https://travis-ci.org/MarkHC/chessgen) |

## Usage examples

### Generating moves

```cpp
using chessgen::Board;
using chessgen::GameOverReason;

Board board; // Creates a game in its default initial position

while(!board.isOver()) {
  auto const legalMoves = board.getLegalMoves();
  auto const r = random() % legalMoves.size();
  auto const moveToPlay = legalMoves[r];
  
  // Gets the algebraic representation for the move
  // and display it
  if (board.getActivePlayer() == Color::White) {
    std::cout << board.getFullMove() << ". " << board.toSAN(moveToPlay);
  } else {
    std::cout << " " << board.toSAN(moveToPlay) << '\n';
  }

  board.makeMove(moveToPlay);
}

switch(board.getGameOverReason()) {
  case GameOverReason::Mate:
    std::cout 
      << std::to_string(~board.getActivePlayer()) 
      << " has won!\n";
    break;
  case GameOverReason::Threefold:
  case GameOverReason::InsuffMaterial:
  case GameOverReason::Stalemate:
    std::cout << "The game was drawn\n";
    break;
}
```

Output of the above code would be something like:
```
1. d3 g6
2. Nf3 Bh6
3. Nfd2 g5
4. a3 e5
5. Nb3 g4
// ...
24. g5 h4+
25. Kxh4 Qf6
26. Be5 Bd7
27. Rf1 Qxg5#
Black has won!
```

You can also give a FEN string to `Board`'s constructor to start the board at any other valid position:

```cpp
using chessgen::Board;

Board board{
  "rn1qk2r/pbp2ppp/1p6/3n4/1b6/2NP1B2/PP2NPPP/R1BQK2R b KQkq - 3 9"
};

std::cout << "The valid moves in this position are: \n";
for(auto&& move : board.getLegalMoves()) {
  std::cout << board.toSAN(move) << '\n';
}
```

### Validating a move

```cpp
using chessgen::Board;

Board board;

assert(board.isValid("e4"));
board.makeMoke("e4");
assert(board.isValid("e4")); // assertion error

```

You can also use from/to squares or castling sides:

```cpp
using chessgen::Board;
using chessgen::Square;
using chessgen::CastleSide;

Board board{
  "rn1qk2r/pbp2ppp/1p6/3n4/1b6/2NP1B2/PP2NPPP/R1BQK2R b KQkq - 3 9"
};

assert(board.isValid(Square::D1, Square::D2));
assert(board.isValid(CastleSide::King));

```

## Installation

Install
```
git clone https://github.com/MarkHC/chessgen.git
cd chessgen
mkdir build && cd build
cmake .. 
make install
```

Build tests
```
git clone --recursive https://github.com/MarkHC/chessgen.git
cd chessgen
mkdir build && cd build
cmake -DCHESSGEN_TEST=ON ..
make
```
