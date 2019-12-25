#include <chessgen/san.hpp>
#include <chessgen/helpers.hpp>

namespace chessgen
{
SANMove::SANMove(File fromFile, Rank fromRank, Square toSquare, Piece promotedTo)
    : mPiece(Piece::Pawn),
      mFromFile(fromFile),
      mFromRank(fromRank),
      mToSquare(toSquare),
      mPromoted(promotedTo),
      mCastling(CastleSide::None)
{
}
SANMove::SANMove(Piece piece, File fromFile, Rank fromRank, Square toSquare)
    : mPiece(piece),
      mFromFile(fromFile),
      mFromRank(fromRank),
      mToSquare(toSquare),
      mPromoted(Piece::None),
      mCastling(CastleSide::None)
{
}
SANMove::SANMove(CastleSide castling)
    : mPiece(Piece::None),
      mFromFile(File::None),
      mFromRank(Rank::None),
      mToSquare(Square::None),
      mPromoted(Piece::None),
      mCastling(castling)
{
}
SANMove SANMove::parse(std::string_view move)
{
  using namespace std::literals;

  auto sanGetPieceType = [](char c) {
    c = std::tolower(c);
    switch (c) {
      // clang-format off
      case 'q': return Piece::Queen;
      case 'r': return Piece::Rook; 
      case 'n': return Piece::Knight; 
      case 'b': return Piece::Bishop;
      case 'k': return Piece::King;
      // clang-format on
      default:
        return Piece::None;
    }
  };

  if (stringEndsWith(move, "e.p."sv)) {
    stringPop(move, 4);
  }

  if (stringEndsWith(move, "#"sv) || stringEndsWith(move, "+"sv)) {
    stringPop(move, 1);
  }

  if (move == "O-O-O"sv || move == "0-0-0"sv) {
    return {CastleSide::Queen};
  } else if (move == "O-O"sv || move == "0-0"sv) {
    return {CastleSide::King};
  }

  auto promotedTo = Piece::None;

  // If the last letter is not a digit, this is a promotion and it indicates
  // the promoted-to piece
  if (!std::isdigit(move.back())) {
    promotedTo = sanGetPieceType(stringPopBack(move));

    if (promotedTo == Piece::None) {
      throw std::runtime_error("Malformed SAN move: " + std::string(move));
    }
    if (move.back() == '=') {
      stringPop(move, 1);
    }
  }

  auto const toRank = stringPopBack(move);
  auto const toFile = stringPopBack(move);

  if (!std::isalpha(toFile)) {
    throw std::runtime_error("Malformed SAN move: " + std::string(move));
  }

  auto const toSquare = makeSquare(File(toFile - 'a'), Rank(toRank - '1'));

  if (move.size() == 0) {
    return {File::None, Rank::None, toSquare, promotedTo};
  }

  bool isCapture = move.back() == 'x';

  if (isCapture) {
    stringPop(move, 1);
  }

  auto fromFile = File::None;
  auto fromRank = Rank::None;

  // Check if a rank number was provided
  if (std::isdigit(move.back())) {
    fromRank = Rank(stringPopBack(move) - '1');
  }

  // If it's a lower case letter, then it's a file
  if (std::islower(move.back())) {
    fromFile = File(stringPopBack(move) - 'a');

    // If there are no more letters, it's a pawn move
    // find the source square and return it
    if (move.size() == 0) {
      return {fromFile, Rank::None, toSquare, promotedTo};
    }
  }
  
  if (promotedTo != Piece::None) {
    // A promotion must be a pawn move
    // and so it cannot reach this code, it must return early
    throw std::runtime_error("Malformed SAN move: " + std::string(move));
  }

  CHESSGEN_ASSERT(std::isupper(move.back()));

  auto const piece = sanGetPieceType(stringPopBack(move));

  CHESSGEN_ASSERT(move.size() == 0);
  
  return {piece, fromFile, fromRank, toSquare};
}
}  // namespace chessgen
