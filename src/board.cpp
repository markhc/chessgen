#include "chessgen/board.hpp"

#include <fmt/format.h>
#include <charconv>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "chessgen/attacks.hpp"
#include "chessgen/helpers.hpp"
#include "chessgen/intrinsics.hpp"
#include "chessgen/rays.hpp"

namespace chessgen
{
static std::string_view _initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static std::once_flag   _flag;

// -------------------------------------------------------------------------------------------------
Board::Board()
{
  std::call_once(_flag, [] {
    rays::precomputeTables();
    attacks::precomputeTables();
  });
  loadFen(_initialFen);
}
// -------------------------------------------------------------------------------------------------
Board::Board(std::string_view initialFen)
{
  std::call_once(_flag, [] {
    rays::precomputeTables();
    attacks::precomputeTables();
  });
  loadFen(initialFen);
}
// -------------------------------------------------------------------------------------------------
void Board::loadFen(std::string_view fen)
{
  clearBitboards();

  auto const fields = stringSplit(fen, ' ');

  if (fields.size() != 6) {
    throw std::runtime_error{fmt::format("Invalid FEN record \"{}\"", fen)};
  }

  auto parsePiecePlacement = [this](std::string_view str) {
    auto boardPos = 56ULL;
    for (auto&& currChar : str) {
      switch (currChar) {
        case 'p':
          mPieces[Color::Black][Piece::Pawn] |= (1ULL << boardPos++);
          break;
        case 'r':
          mPieces[Color::Black][Piece::Rook] |= (1ULL << boardPos++);
          break;
        case 'n':
          mPieces[Color::Black][Piece::Knight] |= (1ULL << boardPos++);
          break;
        case 'b':
          mPieces[Color::Black][Piece::Bishop] |= (1ULL << boardPos++);
          break;
        case 'q':
          mPieces[Color::Black][Piece::Queen] |= (1ULL << boardPos++);
          break;
        case 'k':
          mPieces[Color::Black][Piece::King] |= (1ULL << boardPos++);
          break;
        case 'P':
          mPieces[Color::White][Piece::Pawn] |= (1ULL << boardPos++);
          break;
        case 'R':
          mPieces[Color::White][Piece::Rook] |= (1ULL << boardPos++);
          break;
        case 'N':
          mPieces[Color::White][Piece::Knight] |= (1ULL << boardPos++);
          break;
        case 'B':
          mPieces[Color::White][Piece::Bishop] |= (1ULL << boardPos++);
          break;
        case 'Q':
          mPieces[Color::White][Piece::Queen] |= (1ULL << boardPos++);
          break;
        case 'K':
          mPieces[Color::White][Piece::King] |= (1ULL << boardPos++);
          break;
        case '/':
          boardPos -= 16;  // Go down one rank
          break;
        default:  // handle empty squares
          boardPos += static_cast<std::uint64_t>(currChar - '0');
      }
    }
  };

  auto parsePlayerTurn = [this](std::string_view str) {
    mTurn = str == "w" ? Color::White : Color::Black;
  };

  auto parseCastlingAvailability = [this](std::string_view str) {
    mCastleRights[Color::White] = CastleSide::None;
    mCastleRights[Color::Black] = CastleSide::None;

    if (str != "-") {
      for (auto&& c : str) {
        if (c == 'K') {
          mCastleRights[Color::White] = mCastleRights[Color::White] | CastleSide::King;
        } else if (c == 'Q') {
          mCastleRights[Color::White] = mCastleRights[Color::White] | CastleSide::Queen;
        } else if (c == 'k') {
          mCastleRights[Color::Black] = mCastleRights[Color::Black] | CastleSide::King;
        } else if (c == 'q') {
          mCastleRights[Color::Black] = mCastleRights[Color::Black] | CastleSide::Queen;
        } else {
          throw InvalidFENException{fmt::format("Invalid FEN record. Invalid castle rights '{}'", str)};
        }
      }
    }
  };
  auto parseEnPassantSquare = [this](std::string_view str) {
    if (str != "-") mEnPassant = 1ULL << Move::notationToIndex(str);
  };
  auto parseHalfMoveNumber = [this](std::string_view str) {
    auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), mHalfMoves);
    if (ec != std::errc()) {
      throw std::runtime_error{fmt::format("Invalid FEN record. Invalid half move value '{}'", str)};
    }
  };
  auto parseFullMoveNumber = [this](std::string_view str) {
    auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), mFullMove);
    if (ec != std::errc()) {
      throw std::runtime_error{fmt::format("Invalid FEN record. Invalid full move value '{}'", str)};
    }
  };
  parsePiecePlacement(fields[0]);
  parsePlayerTurn(fields[1]);
  parseCastlingAvailability(fields[2]);
  parseEnPassantSquare(fields[3]);
  parseHalfMoveNumber(fields[4]);
  parseFullMoveNumber(fields[5]);

  updateBitboards();
}
// -------------------------------------------------------------------------------------------------
std::string Board::getFen() const
{
  std::string fen;

  auto getPieceOnSquare = [this](std::uint64_t square) {
    for (auto color : {Color::White, Color::Black}) {
      for (auto piece :
           {Piece::King, Piece::Queen, Piece::Rook, Piece::Bishop, Piece::Knight, Piece::Pawn}) {
        if (mPieces[color][piece] & square) {
          return std::make_pair(color, piece);
        }
      }
    }
    return std::make_pair(Color::White, Piece::None);
  };

  int emptyCount = 0;

  for (auto i = 56; i >= 0; i -= 8) {
    for (int boardPos = i; boardPos < i + 8; boardPos++) {
      auto const square         = 1ULL << boardPos;
      auto const squareOccupied = (square & mOccupied) != 0;
      if (squareOccupied) {
        auto [color, piece] = getPieceOnSquare(square);
        if (emptyCount > 0) {
          fen += std::to_string(emptyCount);
          emptyCount = 0;
        }
        if (color == Color::White)
          fen += to_string<Color::White>(piece);
        else
          fen += to_string<Color::Black>(piece);
      } else {
        emptyCount++;
      }
    }
    if (emptyCount > 0) {
      fen += std::to_string(emptyCount);
      emptyCount = 0;
    }
    if (i != 0) fen += '/';
  }

  fen += mTurn == Color::White ? " w " : " b ";
  if (mCastleRights[Color::White] == CastleSide::None &&
      mCastleRights[Color::Black] == CastleSide::None) {
    fen += '-';
  } else {
    if (enumHasFlag(mCastleRights[Color::White], CastleSide::King)) fen += 'K';
    if (enumHasFlag(mCastleRights[Color::White], CastleSide::Queen)) fen += 'Q';
    if (enumHasFlag(mCastleRights[Color::Black], CastleSide::King)) fen += 'k';
    if (enumHasFlag(mCastleRights[Color::Black], CastleSide::Queen)) fen += 'q';
  }

  fen += ' ';
  fen += mEnPassant == 0 ? "-" : Move::indexToNotation(bitscan_forward(mEnPassant));
  fen += " " + std::to_string(mHalfMoves);
  fen += " " + std::to_string(mFullMove);

  return fen;
}
// -------------------------------------------------------------------------------------------------
std::string Board::prettyPrint(bool useUnicodeChars)
{
  std::string_view unicodePieces[2][6];
  unicodePieces[Color::White][Piece::King]   = useUnicodeChars ? "\u2654" : "K";
  unicodePieces[Color::White][Piece::Queen]  = useUnicodeChars ? "\u2655" : "Q";
  unicodePieces[Color::White][Piece::Rook]   = useUnicodeChars ? "\u2656" : "R";
  unicodePieces[Color::White][Piece::Bishop] = useUnicodeChars ? "\u2657" : "B";
  unicodePieces[Color::White][Piece::Knight] = useUnicodeChars ? "\u2658" : "N";
  unicodePieces[Color::White][Piece::Pawn]   = useUnicodeChars ? "\u2659" : "P";
  unicodePieces[Color::Black][Piece::King]   = useUnicodeChars ? "\u265A" : "k";
  unicodePieces[Color::Black][Piece::Queen]  = useUnicodeChars ? "\u265B" : "q";
  unicodePieces[Color::Black][Piece::Rook]   = useUnicodeChars ? "\u265C" : "r";
  unicodePieces[Color::Black][Piece::Bishop] = useUnicodeChars ? "\u265D" : "b";
  unicodePieces[Color::Black][Piece::Knight] = useUnicodeChars ? "\u265E" : "n";
  unicodePieces[Color::Black][Piece::Pawn]   = useUnicodeChars ? "\u265F" : "p";

  std::stringstream ss;

  auto printIcon = [&](Square square) {
    for (auto color : {Color::White, Color::Black}) {
      for (auto piece :
           {Piece::King, Piece::Queen, Piece::Rook, Piece::Bishop, Piece::Knight, Piece::Pawn}) {
        if (mPieces[color][piece] & square) {
          ss << unicodePieces[color][piece] << ' ';
          return true;
        }
      }
    }
    return false;
  };

  ss << "  +-----------------+\n";
  for (Rank r = Rank::Rank8; r >= Rank::Rank1; --r) {
    ss << static_cast<int>(r) + 1 << " | ";
    for (File f = File::FileA; f <= File::FileH; ++f) {
      auto square = makeSquare(f, r);

      if (!printIcon(square)) ss << ". ";
    }

    ss << "|\n";
  }

  ss << "  +-----------------+\n";
  ss << "    A B C D E F G H" << std::endl;
  return ss.str();
}
// -------------------------------------------------------------------------------------------------
int Board::getHalfMoves() const { return mHalfMoves; }
// -------------------------------------------------------------------------------------------------
int Board::getFullMove() const { return mFullMove; }
// -------------------------------------------------------------------------------------------------
bool Board::isInitialPosition() const { return getFen() == _initialFen; }
// -------------------------------------------------------------------------------------------------
Color Board::getActivePlayer() const { return mTurn; }
// -------------------------------------------------------------------------------------------------
bool Board::isInCheck(Color color) const
{
  auto const kingSquareIndex = bitscan_forward(getPieces(color, Piece::King));

  // no king?
  if (kingSquareIndex == -1) {
    return false;
  }

  return isSquareUnderAttack(~color, squareFromIndex(kingSquareIndex));
}
// -------------------------------------------------------------------------------------------------
bool Board::canShortCastle(Color color) const
{
  // Cannot castle if:
  // - The player has no castle rights (king or rook already moved)
  if (!enumHasFlag(mCastleRights[color], CastleSide::King)) {
    return false;
  }

  auto       kingIndex  = bitscan_forward(getPieces(color, Piece::King));
  auto const enemyColor = ~color;

  auto const squareMask = ((1ULL << (kingIndex + 1)) | (1ULL << (kingIndex + 2)));
  if (getOccupied() & squareMask) return false;

  auto rookIndex = bitscan_reverse(getPieces(color, Piece::Rook));

  // If there is no rook or if the only rook is the wrong one
  if (rookIndex == -1 || rookIndex < kingIndex) return false;

  // Or if one of the squares the king will move to is under attack
  return !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex++)) &&
         !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex++)) &&
         !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex++));
}
// -------------------------------------------------------------------------------------------------
bool Board::canLongCastle(Color color) const
{
  // Cannot castle if:
  // - The player has no castle rights (king or rook already moved)
  if (!enumHasFlag(mCastleRights[color], CastleSide::King)) {
    return false;
  }

  auto       kingIndex  = bitscan_forward(getPieces(color, Piece::King));
  auto const enemyColor = ~color;

  auto const squareMask =
      ((1ULL << (kingIndex - 1)) | (1ULL << (kingIndex - 2) | (1ULL << (kingIndex - 3))));
  if (getOccupied() & squareMask) return false;

  auto rookIndex = bitscan_forward(getPieces(color, Piece::Rook));

  // If there is no rook or if the only rook is the wrong one
  if (rookIndex == -1 || rookIndex > kingIndex) return false;

  // Or if one of the squares the king will move to is under attack
  return !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex--)) &&
         !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex--)) &&
         !isSquareUnderAttack(enemyColor, squareFromIndex(kingIndex--));
}
// -------------------------------------------------------------------------------------------------
CastleSide Board::getCastlingRights(Color color) const { return mCastleRights[color]; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getPieces(Piece type) const
{
  return mPieces[Color::White][type] | mPieces[Color::Black][type];
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getPieces(Color color, Piece type) const { return mPieces[color][type]; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getAllPieces(Color color) const { return mAllPieces[color]; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getOccupied() const { return mOccupied; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getUnoccupied() const { return ~mOccupied; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getEnPassant() const { return mEnPassant; }
// -------------------------------------------------------------------------------------------------
Bitboard Board::getPossibleMoves(Piece type, Color color, Square fromSquare) const
{
  switch (type) {
    case Piece::Pawn:
      if (color == Color::White)
        return getWhitePawnAttacksForSquare(fromSquare);
      else
        return getBlackPawnAttacksForSquare(fromSquare);
    case Piece::Knight:
      return getKnightAttacksForSquare(fromSquare, color);
    case Piece::King:
      return getKingAttacksForSquare(fromSquare, color);
    case Piece::Rook:
      return getRookAttacksForSquare(fromSquare, color);
    case Piece::Bishop:
      return getBishopAttacksForSquare(fromSquare, color);
    case Piece::Queen:
      return getQueenAttacksForSquare(fromSquare, color);
    case Piece::Count:
    default:
      throw std::runtime_error{"Invalid piece type"};
  }
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getKingBlockers(Color them) const
{
  auto       blockers = Bitboard{};
  auto const us       = ~them;

  auto       ksIndex         = bitscan_forward(getPieces(them, Piece::King));
  auto const rooksOrQueens   = getPieces(us, Piece::Queen) | getPieces(us, Piece::Rook);
  auto const bishopsOrQueens = getPieces(us, Piece::Queen) | getPieces(us, Piece::Rook);
  auto const rqAttacks =
      attacks::getSlidingAttacks(Piece::Rook, squareFromIndex(ksIndex), 0ULL) & rooksOrQueens;
  auto const bqAttacks =
      attacks::getSlidingAttacks(Piece::Bishop, squareFromIndex(ksIndex), 0ULL) & bishopsOrQueens;

  // Find all sliders aiming towards the king position
  auto sliders = getAllPieces(us) & (rqAttacks | bqAttacks);

  // Mask the sliders out of the occupancy bits
  auto const occupancy = getOccupied() ^ sliders;

  auto more_than_one = [](Bitboard b) constexpr { return b & (b - 1); };

  while (sliders) {
    auto const sniperSq = pop_lsb(sliders);
    auto const b = BBGetBetween(squareFromIndex(ksIndex), squareFromIndex(sniperSq)) & occupancy;

    if (b && !more_than_one(b)) {
      blockers |= b;
      // if (b & getPieces(color_of(getPieceOn(squareFromIndex(ksIndex)))))
      //  pinners |= sniperSq;
    }
  }
  return blockers;
}
// -------------------------------------------------------------------------------------------------
bool Board::isSquareUnderAttack(Color color, Square square) const
{
  //
  // To check if a square is under attack we check if a piece on this sqaure can attack the same
  // enemy piece If that is true, then the enemy piece is also attacking this square
  //
  if (attacks::getNonSlidingAttacks(Piece::Pawn, square, ~color) & getPieces(color, Piece::Pawn))
    return true;
  if (attacks::getNonSlidingAttacks(Piece::Knight, square, Color::White) &
      getPieces(color, Piece::Knight))
    return true;
  if (attacks::getNonSlidingAttacks(Piece::King, square, Color::White) & getPieces(color, Piece::King))
    return true;

  auto const bishopsQueens = getPieces(color, Piece::Bishop) | getPieces(color, Piece::Queen);
  if (attacks::getSlidingAttacks(Piece::Bishop, square, getOccupied()) & bishopsQueens) {
    return true;
  }

  auto const rooksQueens = getPieces(color, Piece::Rook) | getPieces(color, Piece::Queen);
  if (attacks::getSlidingAttacks(Piece::Rook, square, getOccupied()) & rooksQueens) {
    return true;
  }

  return false;
}
// -------------------------------------------------------------------------------------------------
Piece Board::getPieceOn(Square sq) const
{
  auto square = 1ULL << indexFromSquare(sq);
  for (auto color = 0; color < 2; ++color) {
    for (auto pt = 0; pt < 6; ++pt) {
      if (mPieces[color][pt] & square) return Piece(pt);
    }
  }
  return Piece::None;
}
// -------------------------------------------------------------------------------------------------
void Board::addPiece(Piece type, Color color, Square square)
{
  // clang-format off
  mPieces[color][type] |= 1ULL << indexFromSquare(square); 
  mAllPieces[color]    |= 1ULL << indexFromSquare(square);
  mOccupied            |= 1ULL << indexFromSquare(square);
  // clang-format on
}
// -------------------------------------------------------------------------------------------------
void Board::removePiece(Piece type, Color color, Square square)
{
  // clang-format off
  mPieces[color][type] &= ~(1ULL << indexFromSquare(square));
  mAllPieces[color]    &= ~(1ULL << indexFromSquare(square));
  mOccupied            &= ~(1ULL << indexFromSquare(square));
  // clang-format on
}
// -------------------------------------------------------------------------------------------------
void Board::clearBitboards()
{
  for (Color color : {Color::White, Color::Black}) {
    for (Piece Piece :
         {Piece::Pawn, Piece::Knight, Piece::Bishop, Piece::Rook, Piece::Queen, Piece::King}) {
      mPieces[color][Piece] = 0ULL;
    }

    mAllPieces[color] = 0ULL;
  }

  mEnPassant = 0ULL;
  mOccupied  = 0ULL;
}
// -------------------------------------------------------------------------------------------------
void Board::updateBitboards()
{
  mAllPieces[Color::White] =
      mPieces[Color::White][Piece::Pawn] | mPieces[Color::White][Piece::Rook] |
      mPieces[Color::White][Piece::Knight] | mPieces[Color::White][Piece::Bishop] |
      mPieces[Color::White][Piece::Queen] | mPieces[Color::White][Piece::King];

  mAllPieces[Color::Black] =
      mPieces[Color::Black][Piece::Pawn] | mPieces[Color::Black][Piece::Rook] |
      mPieces[Color::Black][Piece::Knight] | mPieces[Color::Black][Piece::Bishop] |
      mPieces[Color::Black][Piece::Queen] | mPieces[Color::Black][Piece::King];

  mOccupied = mAllPieces[Color::White] | mAllPieces[Color::Black];
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getWhitePawnAttacksForSquare(Square square) const
{
  return attacks::getNonSlidingAttacks(Piece::Pawn, square, Color::White);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getBlackPawnAttacksForSquare(Square square) const
{
  return attacks::getNonSlidingAttacks(Piece::Pawn, square, Color::Black);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getKingAttacksForSquare(Square square, Color color) const
{
  return attacks::getNonSlidingAttacks(Piece::King, square, color) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getKnightAttacksForSquare(Square square, Color color) const
{
  return attacks::getNonSlidingAttacks(Piece::Knight, square, color) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getBishopAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(Piece::Bishop, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getRookAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(Piece::Rook, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard Board::getQueenAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(Piece::Queen, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
}  // namespace chessgen
