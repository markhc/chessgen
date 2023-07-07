
// #include <gtest/gtest.h>

// #include <chessgen/board.hpp>

// using namespace chessgen;

// static int notationToIndex(std::string_view notation)
// {
//   return static_cast<int>(makeSquare(File(notation[0] - 'a'), Rank(notation[1] - '1')));
// }
// static std::string indexToNotation(int index)
// {
//   using std::to_string;

//   return to_string(Square(index));
// }
// int main()
// {
//   using std::to_string;

//   Board board("rnbqkb1r/pp2pppp/2p2n2/3p4/3P4/4PN2/PPP2PPP/RNBQKB1R w KQkq - 2 4");

//   std::string moves[] = {"Bd3", "Nbd7", "O-O", "h6"};

//   std::cout << board.prettyPrint(false) << std::endl;

//   for (auto move : moves) {
//     if (board.isValid(move)) {
//       board.makeMove(move);
//     } else {
//       std::cout << "\nInvalid Move!";
//       break;
//     }

//     std::cout << board.prettyPrint(false) << std::endl;
//     std::cout << board.getFen() << std::endl;
//   }

//   for (auto state : board.getGameHistory()) {
//     if (state.boardState.getActivePlayer() == Color::ColorWhite) {
//       std::cout << std::endl << state.boardState.getFullMove() << ". ";
//     }
//     if (state.movePlayed.has_value())
//       std::cout << state.boardState.toSAN(state.movePlayed.value()) << " ";
//   }
// }
