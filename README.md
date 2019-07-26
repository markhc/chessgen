# cppgen

### A chess move generator and validator written in modern C++

cppgen is a C++17 chess library that is capable of generating legal moves or validating moves for a given chess position.

It is both fast and easy to use.

This is still an early version, so bugs might (and probably do) exist. Please fill an issue if you find any.

## Usage examples

### Generating moves

```
using cppgen::Board;
using cppgen::Color;

Board board; // Creates a game in its default initial position

while(!board.isMate() && !board.isInsufficientMaterial()) {
  auto const legalMoves = board.getLegalMoves();
  auto const moveToPlay = random() % legalMoves.size();
  
  // Gets the algebraic representation for the move
  // and display it
  std::cout 
    << "Playing " 
    << board.toSAN(legalMoves[moveToPlay]) << '\n';

  board.makeMove(legalMoves[moveToPlay]);
}
if(board.isInsufficientMaterial())  {
  std::cout << "Game was drawn\n";
} else {
  std::cout 
    << board.getActivePlayer() == White ? "Black" : "White" 
    << " has won the game by checkmate!\n";
}

```

You can also give a FEN string to `Board`'s constructor to start the board at any other valid position:

```
using cppgen::Board;
using cppgen::Color;

Board board{
  "rn1qk2r/pbp2ppp/1p6/3n4/1b6/2NP1B2/PP2NPPP/R1BQK2R b KQkq - 3 9"
};

std::cout << "The valid moves in this position are: \n";
for(auto&& move : board.getLegalMoves()) {
  std::cout << board.toSAN(move) << '\n';
}
```

### Validating a move


```
using cppgen::Board;

Board board;

assert(board.isValid("e4"));
board.makeMoke("e4");
assert(board.isValid("e4")); // assertion error

```

You can also use from/to squares or castling sides:

```
using cppgen::Board;
using cppgen::Square;
using cppgen::CastleSide;

Board board{
  "rn1qk2r/pbp2ppp/1p6/3n4/1b6/2NP1B2/PP2NPPP/R1BQK2R b KQkq - 3 9"
};

assert(board.isValid(Square::D1, Square::D2));
assert(board.isValid(CastleSide::King));

```




