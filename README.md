# cppgen

### A chess move generator and validator written in modern C++

# STILL A WORK IN PROGRESS! COME BACK IN A FEW DAYS!

cppgen is a C++17 chess library that is capable of generating legal moves or validating moves for a given chess position.

It is both fast and easy to use.

This is still an early version, so bugs might (and probably do) exist. Please fill an issue if you find any.

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
  std::cout << "Playing move " << board.toSAN(moveToPlay) << '\n';

  board.makeMove(moveToPlay);
}

switch(board.getGameOverReason()) {
  case GameOverReason::Mate:
    std::cout 
      << std::to_string(~board.getActivePlayer()) 
      << " has won!\n";
    break;
  case GameOverReason::ThreeFold:
  case GameOverReason::InsuffMaterial:
  case GameOverReason::Stalemate:
    std::cout << "The game was drawn\n";
    break;
}

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
```cpp
// Creates a board in the default initial position
Board();

// Creates a board from the given position
Board(std::string_view fen);
```

### Board::loadFen(std::string_view fen)
// TODO
### Board::getFen()
// TODO
### Board::prettyPrint(bool useUnicodeChars)
// TODO
### Board::getLegalMoves()
// TODO
### Board::getLegalMovesForSquare(Square square)
// TODO
### Board::isValid(Move const& move) 
// TODO
### Board::toSAN(Move)
// TODO
### Board::makeMove(Move const& move)
// TODO
### Board::makeMove(std::string_view move)
// TODO
### Board::isOver()
// TODO
### Board::getGameOverReason()
// TODO
### Board::getActivePlayer()
// TODO
### Board::isInCheck()
// TODO




