#include "chessgen/move.hpp"
#include "chessgen/helpers.hpp"

#include <fmt/format.h>

namespace chessgen
{
using namespace std::literals;

// -----------------------------------------------------------------------------
static constexpr bool isUppercase(char c) { return c >= 'A' && c <= 'Z'; }
// -----------------------------------------------------------------------------
static Piece pieceTypeFromChar(char c)
{
  c = std::tolower(c);
  switch (c) {
    case 'b':
      return Piece::Bishop;
    case 'k':
      return Piece::King;
    case 'n':
      return Piece::Knight;
    case 'r':
      return Piece::Rook;
    case 'q':
      return Piece::Queen;
    default:
      return Piece::None;
  }
}
// -----------------------------------------------------------------------------
Move::MoveInfo::MoveInfo() { new (&pieceMove) PieceMove(); }
// -----------------------------------------------------------------------------
Move::Move(std::string_view san) : mInfo{}
{
  if (stringContains(san, '+')) {
    mIsCheck = true;
    stringPopBack(san);
  } else if (stringContains(san, '#')) {
    mIsMate = true;
    stringPopBack(san);
  }

  if (san == "O-O" || san == "0-0") {
    mType = MoveType::ShortCastle;
  } else if (san == "O-O-O" || san == "0-0-0") {
    mType = MoveType::LongCastle;
  } else if (isUppercase(san.front())) {
    parsePieceMove(san);
  } else {
    parsePawnMove(san);
  }
}
// -----------------------------------------------------------------------------
void Move::parsePawnMove(std::string_view san)
{
  // <pawn capture> ::= <from file>[<from rank>] 'x' <to square>['=' <promoted to>]
  // <square>       ::= <file letter><rank number>
  // <promoted to>  ::= 'P' | 'N' | 'B' | 'R' | 'Q'

  auto const origsan = san;  // save a copy for throwing purposes
  mType              = MoveType::PawnMove;

  if (stringEndsWith(san, "e.p."sv)) {
    // Drop the optional En Passant suffix
    stringPop(san, 4);
  }

  // if the last letter is not a number, this is a pawn promotion
  mInfo.pawnMove.isPromotion = std::isalpha(san.back());
  if (mInfo.pawnMove.isPromotion) {
    if (auto piece = pieceTypeFromChar(stringPopBack(san)); piece != 0) {
      mInfo.pawnMove.promotedTo = piece;
    } else {
      throw InvalidSANException{origsan};
    }
    if (san.back() == '=') stringPopBack(san);  // drop the equal sign if it exists
  }

  auto const rank         = stringPopBack(san);
  auto const file         = stringPopBack(san);
  mInfo.pawnMove.toSquare = makeSquare(File(file - 'a'), Rank(rank - '0'));

  mInfo.pawnMove.fromFile = File::None;
  if (san.size() > 0 && san.back() == 'x') {
    mInfo.pawnMove.isCapture = true;
    stringPopBack(san);

    mInfo.pawnMove.fromFile = File(stringPopBack(san) - 'a');
  }
}
// -----------------------------------------------------------------------------
void Move::parsePieceMove(std::string_view san)
{
  // <piece moves> ::= <Piece>[<from file>|<from rank>|<from square>]['x']<to square>
  // <square>      ::= <file letter><rank number>

  auto const origsan       = san;
  auto const toRank        = stringPopBack(san);
  auto const toFile        = stringPopBack(san);
  mType                    = MoveType::PieceMove;
  mInfo.pieceMove.toSquare = makeSquare(File(toFile - 'a'), Rank(toRank - '0'));

  if (san.back() == 'x') {
    mInfo.pieceMove.isCapture = true;
    stringPopBack(san);
  }

  mInfo.pieceMove.fromRank = Rank::None;
  mInfo.pieceMove.fromFile = File::None;

  if (std::isdigit(san.back())) {
    mInfo.pieceMove.fromRank = Rank(stringPopBack(san) - '0');
  }
  if (std::islower(san.back())) {
    mInfo.pieceMove.fromFile = File(stringPopBack(san) - 'a');
  }

  auto const piece = pieceTypeFromChar(stringPopBack(san));

  if (piece == Piece::None) {
    throw InvalidSANException{origsan};
  }

  mInfo.pieceMove.piece = piece;
}
// -----------------------------------------------------------------------------
File Move::fromFile() const
{
  switch (mType) {
    case MoveType::PieceMove:
      return mInfo.pieceMove.fromFile;
    case MoveType::PawnMove:
      return mInfo.pawnMove.fromFile;
    case MoveType::LongCastle:
    case MoveType::ShortCastle:
    case MoveType::Unknown:
    default:
      throw std::runtime_error{"Source square information is not available for this type of move"};
  }
}
// -----------------------------------------------------------------------------
Square Move::toSquare() const
{
  switch (mType) {
    case MoveType::PieceMove:
      return mInfo.pieceMove.toSquare;
    case MoveType::PawnMove:
      return mInfo.pawnMove.toSquare;
    case MoveType::LongCastle:
    case MoveType::ShortCastle:
    case MoveType::Unknown:
    default:
      throw std::runtime_error{"Target square information is not available for this type of move"};
  }
}
// -----------------------------------------------------------------------------
Piece Move::pieceType() const
{
  switch (mType) {
    case MoveType::PawnMove:
      return Piece::Pawn;
    case MoveType::PieceMove:
      return mInfo.pieceMove.piece;
    case MoveType::LongCastle:
    case MoveType::ShortCastle:
    case MoveType::Unknown:
    default:
      return {};
  }
}
// -----------------------------------------------------------------------------
bool Move::isCapture() const
{
  switch (mType) {
    case MoveType::PawnMove:
      return mInfo.pawnMove.isCapture;
    case MoveType::PieceMove:
      return mInfo.pieceMove.isCapture;
    case MoveType::LongCastle:
    case MoveType::ShortCastle:
    case MoveType::Unknown:
    default:
      return false;
  }
}
// -----------------------------------------------------------------------------
bool Move::isCheck() const { return mIsCheck; }
// -----------------------------------------------------------------------------
bool Move::isMate() const { return mIsMate; }
// -----------------------------------------------------------------------------
bool Move::isPromotion() const { return mType == MoveType::PawnMove && mInfo.pawnMove.isPromotion; }
// -----------------------------------------------------------------------------
bool Move::isShortCastle() const { return mType == MoveType::ShortCastle; }
// -----------------------------------------------------------------------------
bool Move::isLongCastle() const { return mType == MoveType::LongCastle; }
// -----------------------------------------------------------------------------
bool Move::isCastle() const { return isShortCastle() || isLongCastle(); }
// -----------------------------------------------------------------------------
Piece Move::promotedTo() const
{
  if (isPromotion()) {
    return mInfo.pawnMove.promotedTo;
  }
  return {};
}
// -----------------------------------------------------------------------------
std::string Move::toSan() const
{
  std::string result;

  switch (mType) {
    case MoveType::PawnMove:
      if (mInfo.pawnMove.fromFile != File::None) {
        result += to_string(mInfo.pawnMove.fromFile);
      }
      if (mInfo.pawnMove.isCapture) {
        result += 'x';
      }
      result += to_string(mInfo.pawnMove.toSquare);
      if (mInfo.pawnMove.isPromotion) {
        result += '=';
        result += to_string(mInfo.pawnMove.promotedTo);
      }
      if (mIsCheck) {
        result += '+';
      } else if (mIsMate) {
        result += '#';
      }
      return result;
    case MoveType::PieceMove:
      result += to_string(mInfo.pieceMove.piece);
      if (mInfo.pieceMove.fromFile != File::None) {
        result += to_string(mInfo.pieceMove.fromFile);
      }
      if (mInfo.pieceMove.fromRank != Rank::None) {
        result += to_string(mInfo.pieceMove.fromRank);
      }
      if (mInfo.pieceMove.isCapture) {
        result += 'x';
      }
      result += to_string(mInfo.pieceMove.toSquare);
      if (mIsCheck) {
        result += '+';
      } else if (mIsMate) {
        result += '#';
      }
      return result;
    case MoveType::LongCastle:
      return "O-O-O";
    case MoveType::ShortCastle:
      return "O-O";
    case MoveType::Unknown:
    default:
      return "-";
  }
}
// -----------------------------------------------------------------------------
}  // namespace chessgen
