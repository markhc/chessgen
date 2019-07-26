# cppgen 

[![Build Status](https://travis-ci.org/MarkHC/cppgen.svg?branch=master)](https://travis-ci.org/MarkHC/cppgen)

### A chess move generator and validator written in modern C++

cppgen is a C++17 chess library that is capable of generating legal moves or validating moves for a given chess position.

It is both fast and easy to use.

This is still an early version, so bugs might (and probably do) exist. Please fill an issue if you find any.

## Installation
// TODO

## Usage examples

### Generating moves

```cpp
using cppgen::Board;
using cppgen::GameOverReason;

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
using cppgen::Board;

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
using cppgen::Board;

Board board;

assert(board.isValid("e4"));
board.makeMoke("e4");
assert(board.isValid("e4")); // assertion error

```

You can also use from/to squares or castling sides:

```cpp
using cppgen::Board;
using cppgen::Square;
using cppgen::CastleSide;

Board board{
  "rn1qk2r/pbp2ppp/1p6/3n4/1b6/2NP1B2/PP2NPPP/R1BQK2R b KQkq - 3 9"
};

assert(board.isValid(Square::D1, Square::D2));
assert(board.isValid(CastleSide::King));

```

## Main API

### Constructors

Creates a new board
```cpp
// Creates a board in the default initial position
Board();

// Creates a board from the given position
Board(std::string_view fen);
```

### Board::loadFen(std::string_view)
Sets the board to the position provided by the FEN string.
```cpp
// Creates a board in the default initial position
Board board;

// Load a new position
board.loadFen("rnbq1knr/ppppp1pp/8/8/8/8/PPPPP1PP/RNBQK2R w KQkq - 0 1");
```

### Board::getFen()
Returns a FEN string for the current board position
```cpp
Board board;

board.getFen(); 
// returns "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

### Board::prettyPrint(bool useUnicodeChars = true)
Returns a pretty string representation for the current board position.
```cpp
Board board;

board.prettyPrint(); 
//   +-----------------+
// 8 | ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ |
// 7 | ♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ |
// 6 | . . . . . . . . |
// 5 | . . . . . . . . |
// 4 | . . . . . . . . |
// 3 | . . . . . . . . |
// 2 | ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ |
// 1 | ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ |
//   +-----------------+
//     A B C D E F G H

board.prettyPrint(false); 
//   +-----------------+
// 8 | r n b q k b n r |
// 7 | p p p p p p p p |
// 6 | . . . . . . . . |
// 5 | . . . . . . . . |
// 4 | . . . . . . . . |
// 3 | . . . . . . . . |
// 2 | P P P P P P P P |
// 1 | R N B Q K B N R |
//   +-----------------+
//     A B C D E F G H
```
### Board::getLegalMoves()
Returns the set of legal moves for the current board position.

### Board::getLegalMovesAsSAN()
Returns the set of legal moves for the current board position as the SAN representation.

### Board::getLegalMovesForSquare(Square)
Returns the set of legal moves starting from the given square.
```cpp
Board board;

board.getLegalMovesForSquare(Square::E2);
// returns:
//   e2e3
//   e2e4
```

### Board::isValid(std::string_view)
### Board::isValid(Square, Square)
### Board::isValid(CastleSide)
### Board::isValid(Move)
Checks whether the given move is in the set of legal moves.

### Board::toSAN(Move)
Returns the Standard Algebraic Notation string for the given Move object.

**IMPORTANT**: To generate a correct SAN string the move MUST be a legal move on the current board position.
The behavior of this function is undefined for illegal moves.

### Board::makeMove(Move)
### Board::makeMove(std::string_view)
Plays a move, modifying the board position.

### Board::isOver()
Checks if the game has ended by either a Checkmate, Stalemate, Insufficient material of Threefold repetition (this last one is not yet supported)

### Board::getGameOverReason()
If the game has ended, returns the reason (Mate, Stalemate, etc). If the game is still underway returns `GameOverReason::OnGoing`

### Board::isInCheck()
Returns true if the currently active player is under check




