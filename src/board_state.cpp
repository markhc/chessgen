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

#include "chessgen/board_state.hpp"

#include <charconv>
#include <cassert>

#include "chessgen/attacks.hpp"
#include "chessgen/board.hpp"
#include "chessgen/helpers.hpp"

namespace chessgen
{
// -------------------------------------------------------------------------------------------------
static int notationToIndex(std::string_view notation)
{
  return static_cast<int>(makeSquare(File(notation[0] - 'a'), Rank(notation[1] - '1')));
}
// -------------------------------------------------------------------------------------------------
static std::string indexToNotation(int index)
{
  using std::to_string;

  return to_string(Square(index));
}
// -------------------------------------------------------------------------------------------------
void BoardState::clearEnPassant()
{
  mEnPassant.clear();
}
// -------------------------------------------------------------------------------------------------
void BoardState::updateNonPieceBitboards()
{
  mAllPieces[ColorWhite] =
      mPieces[ColorWhite][PiecePawn] | mPieces[ColorWhite][PieceRook] |
      mPieces[ColorWhite][PieceKnight] | mPieces[ColorWhite][PieceBishop] |
      mPieces[ColorWhite][PieceQueen] | mPieces[ColorWhite][PieceKing];

  mAllPieces[ColorBlack] =
      mPieces[ColorBlack][PiecePawn] | mPieces[ColorBlack][PieceRook] |
      mPieces[ColorBlack][PieceKnight] | mPieces[ColorBlack][PieceBishop] |
      mPieces[ColorBlack][PieceQueen] | mPieces[ColorBlack][PieceKing];

  mOccupied = mAllPieces[ColorWhite] | mAllPieces[ColorBlack];
}
// -------------------------------------------------------------------------------------------------
std::string BoardState::getFen() const
{
  std::string fen;

  int emptyCount = 0;

  for (auto i = 56; i >= 0; i -= 8) {
    for (int boardPos = i; boardPos < i + 8; boardPos++) {
      auto const square         = makeSquare(boardPos);
      auto const squareOccupied = !!(mOccupied & square);
      if (squareOccupied) {
        auto [piece, color] = getPieceOn(square);
        if (emptyCount > 0) {
          fen += std::to_string(emptyCount);
          emptyCount = 0;
        }
        if (color == ColorWhite)
          fen += to_string<ColorWhite>(piece);
        else
          fen += to_string<ColorBlack>(piece);
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

  fen += mTurn == ColorWhite ? " w " : " b ";
  if (mCastleRights[ColorWhite] == CastleSide::None &&
      mCastleRights[ColorBlack] == CastleSide::None) {
    fen += '-';
  } else {
    if (enumHasFlag(mCastleRights[ColorWhite], CastleSide::King)) fen += 'K';
    if (enumHasFlag(mCastleRights[ColorWhite], CastleSide::Queen)) fen += 'Q';
    if (enumHasFlag(mCastleRights[ColorBlack], CastleSide::King)) fen += 'k';
    if (enumHasFlag(mCastleRights[ColorBlack], CastleSide::Queen)) fen += 'q';
  }

  fen += ' ';
  fen += !mEnPassant ? "-" : indexToNotation(mEnPassant.lsb());
  fen += " " + std::to_string(mHalfMoves);
  fen += " " + std::to_string(mFullMove);

  return fen;
}
// -------------------------------------------------------------------------------------------------
BoardState BoardState::fromFen(std::string_view view, ChessVariant variant)
{
  auto const fields = stringSplit(view, ' ');

  auto parsePiecePlacement = [&](BoardState& state, std::string_view str) {
    auto boardPos = 56ULL;

    auto getPieceInfo = [](char c) -> std::pair<Color, Piece> {
      switch (c) {
        case 'p':
          return std::make_pair(ColorBlack, PiecePawn);
        case 'r':
          return std::make_pair(ColorBlack, PieceRook);
        case 'n':
          return std::make_pair(ColorBlack, PieceKnight);
        case 'b':
          return std::make_pair(ColorBlack, PieceBishop);
        case 'q':
          return std::make_pair(ColorBlack, PieceQueen);
        case 'k':
          return std::make_pair(ColorBlack, PieceKing);
        case 'P':
          return std::make_pair(ColorWhite, PiecePawn);
        case 'R':
          return std::make_pair(ColorWhite, PieceRook);
        case 'N':
          return std::make_pair(ColorWhite, PieceKnight);
        case 'B':
          return std::make_pair(ColorWhite, PieceBishop);
        case 'Q':
          return std::make_pair(ColorWhite, PieceQueen);
        case 'K':
          return std::make_pair(ColorWhite, PieceKing);
        default:
          assert(!"Logic error");
          return {};
      }
    };

    for (auto&& currChar : str) {
      if ((currChar != '/' && boardPos >= 64) || boardPos > 64) {
        throw std::runtime_error{"Malformed FEN string"};
      }

      switch (currChar) {
        case 'p':
        case 'r':
        case 'n':
        case 'b':
        case 'q':
        case 'k':
        case 'P':
        case 'R':
        case 'N':
        case 'B':
        case 'Q':
        case 'K': {
          auto [color, piece] = getPieceInfo(currChar);

          state.mPieces[color][piece].setBit(boardPos++);
          break;
        }
        case '/':
          boardPos -= 16;  // Go down one rank
          break;
          // clang-format off
      case '1': case '2': case '3': 
      case '4': case '5': case '6':  
      case '7': case '8': case '9': 
        boardPos += static_cast<std::uint64_t>(currChar) - '0';
        break;
        // clang-format on
        default:
          throw std::runtime_error{"Malformed FEN string"};
      }
    }

    if (boardPos != 8) {
      throw std::runtime_error{"Malformed FEN string"};
    }
  };

  auto parsePlayerTurn = [&](BoardState& state, std::string_view str) {
    if (str == "w")
      state.mTurn = ColorWhite;
    else if (str == "b")
      state.mTurn = ColorBlack;
    else
      throw std::runtime_error{"Invalid play turn"};
  };

  auto parseCastlingAvailability = [&](BoardState& state, std::string_view str) {
    if (str != "-") {
      for (auto&& c : str) {
        if (c == 'K') {
          state.mCastleRights[ColorWhite] = state.mCastleRights[ColorWhite] | CastleSide::King;
        } else if (c == 'Q') {
          state.mCastleRights[ColorWhite] = state.mCastleRights[ColorWhite] | CastleSide::Queen;
        } else if (c == 'k') {
          state.mCastleRights[ColorBlack] = state.mCastleRights[ColorBlack] | CastleSide::King;
        } else if (c == 'q') {
          state.mCastleRights[ColorBlack] = state.mCastleRights[ColorBlack] | CastleSide::Queen;
        } else {
          throw std::runtime_error{"Invalid castling rights"};
        }
      }
    }
  };

  auto parseEnPassantSquare = [&](BoardState& state, std::string_view str) {
    if (str != "-") {
      if (str[0] < 'a' || str[0] > 'h' || str[1] < '1' || str[1] > '8')
        throw std::runtime_error{"Invalid EP square"};

      state.mEnPassant.setBit(notationToIndex(str));
    }
  };

  auto parseHalfMoveNumber = [&](BoardState& state, std::string_view str) {
    auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), state.mHalfMoves);
    if (ec != std::errc()) {
      throw std::runtime_error{"Invalid half move value"};
    }
  };

  auto parseFullMoveNumber = [&](BoardState& state, std::string_view str) {
    auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), state.mFullMove);
    if (ec != std::errc()) {
      throw std::runtime_error{"Invalid full move value"};
    }
  };

  BoardState state;

  if (variant == ChessVariant::Standard) {
    if (fields.size() == 6) {
      parsePiecePlacement(state, fields[0]);
      parsePlayerTurn(state, fields[1]);
      parseCastlingAvailability(state, fields[2]);
      parseEnPassantSquare(state, fields[3]);
      parseHalfMoveNumber(state, fields[4]);
      parseFullMoveNumber(state, fields[5]);
    } else if (fields.size() == 4) {
      parsePiecePlacement(state, fields[0]);
      parsePlayerTurn(state, fields[1]);
      parseCastlingAvailability(state, fields[2]);
      parseEnPassantSquare(state, fields[3]);
      state.mHalfMoves = 0;
      state.mFullMove  = 1;
    } else {
      throw std::runtime_error{"Malformed FEN string"};
    }
  }

  state.updateNonPieceBitboards();

  return state;
}
// -------------------------------------------------------------------------------------------------
std::string BoardState::getSanForMove(UCIMove const& move) const
{
  using namespace std::literals;
  using std::to_string;

  auto const us   = mTurn;
  auto const from = move.fromSquare();
  auto const to   = move.toSquare();

  auto appendSuffixes = [&](std::string san) {
    if (isMoveMate(move))
      san += '#';
    else if (isMoveCheck(move))
      san += '+';
    return san;
  };

  if (move.isCastling()) {
    if (move.getCastleSide() == CastleSide::King) {
      return appendSuffixes("O-O");
    } else {
      return appendSuffixes("O-O-O");
    }
  }

  auto const piece     = getPieceOn(from).type;
  auto const isCapture = isSquareEmpty(to);

  if (piece == PieceNone) return "";

  if (piece == PiecePawn) {
    if (!isCapture) {
      return appendSuffixes(to_string(to));
    } else {
      return appendSuffixes(to_string(getFile(from)) + 'x' + to_string(to));
    }
  }
  // If there are 2 pieces in position to make this move, we need to disambiguate it
  auto attackers = getAttackers(us, to) & getPieces(us, piece);

  CHESSGEN_ASSERT(!attackers.isZero());

  auto san = [piece]() {
    switch (piece) {
      case PieceBishop:
        return "B"s;
      case PieceKnight:
        return "N"s;
      case PieceRook:
        return "R"s;
      case PieceQueen:
        return "Q"s;
      case PieceKing:
        return "K"s;

      // These should not happen
      case PiecePawn:
      case PieceNone:
      default:
        return "-"s;
    }
  }();

  if (attackers.popCount() == 1) {
    // Unambiguous
    if (isCapture) {
      san += 'x';
    }

    return appendSuffixes(san + to_string(to));
  } else if (attackers.popCount() == 2) {
    // 2 pieces attacking, disambiguate them by file or rank
    auto attacker1 = makeSquare(attackers.popLsb());
    auto attacker2 = makeSquare(attackers.popLsb());

    if (getFile(attacker1) != getFile(attacker2))
      san += to_string(getFile(from));
    else
      san += to_string(getRank(from));

    if (isCapture) {
      san += 'x';
    }

    return appendSuffixes(san + to_string(to));
  } else {
    san += to_string(from);

    if (isCapture) {
      san += 'x';
    }
    // 3 pieces or more require the full square for disambiguation
    return appendSuffixes(san + to_string(to));
  }
}
// -------------------------------------------------------------------------------------------------
int BoardState::getHalfMoves() const
{
  return mHalfMoves;
}
// -------------------------------------------------------------------------------------------------
int BoardState::getFullMove() const
{
  return mFullMove;
}
// -------------------------------------------------------------------------------------------------
Color BoardState::getActivePlayer() const
{
  return mTurn;
}
// -------------------------------------------------------------------------------------------------
bool BoardState::isInCheck() const
{
  auto const kingSquareIndex = getKingSquare(getActivePlayer());

  // no king?
  if (kingSquareIndex == Square::None) {
    return false;
  }

  return isSquareUnderAttack(~getActivePlayer(), kingSquareIndex);
}
// -------------------------------------------------------------------------------------------------
bool BoardState::canShortCastle(Color color) const
{
  // Cannot castle if:
  // - The player has no castle rights (king or rook already moved)
  if (!enumHasFlag(mCastleRights[color], CastleSide::King)) {
    return false;
  }

  auto       kingIndex  = getPieces(color, PieceKing).lsb();
  auto const enemyColor = ~color;

  auto const squareMask = Bitboard((1ULL << (kingIndex + 1)) | (1ULL << (kingIndex + 2)));
  if (getOccupied() & squareMask) return false;

  auto rookIndex = getPieces(color, PieceRook).msb();

  // If there is no rook or if the only rook is the wrong one
  if (rookIndex == -1 || rookIndex < kingIndex) return false;

  // Or if one of the squares the king will move to is under attack
  return !isSquareUnderAttack(enemyColor, makeSquare(kingIndex++)) &&
         !isSquareUnderAttack(enemyColor, makeSquare(kingIndex++)) &&
         !isSquareUnderAttack(enemyColor, makeSquare(kingIndex++));
}
// -------------------------------------------------------------------------------------------------
bool BoardState::canLongCastle(Color color) const
{
  // Cannot castle if:
  // - The player has no castle rights (king or rook already moved)
  if (!enumHasFlag(mCastleRights[color], CastleSide::Queen)) {
    return false;
  }

  auto       kingIndex  = getPieces(color, PieceKing).msb();
  auto const enemyColor = ~color;

  auto const squareMask = Bitboard((1ULL << (kingIndex - 1)) |  //
                                   (1ULL << (kingIndex - 2)) |  //
                                   (1ULL << (kingIndex - 3)));
  if (getOccupied() & squareMask) return false;

  auto rookIndex = getPieces(color, PieceRook).lsb();

  // If there is no rook or if the only rook is the wrong one
  if (rookIndex == -1 || rookIndex > kingIndex) return false;

  // Or if one of the squares the king will move to is under attack
  return !isSquareUnderAttack(enemyColor, makeSquare(kingIndex--)) &&
         !isSquareUnderAttack(enemyColor, makeSquare(kingIndex--)) &&
         !isSquareUnderAttack(enemyColor, makeSquare(kingIndex--));
}
// -------------------------------------------------------------------------------------------------
CastleSide BoardState::getCastlingRights(Color color) const
{
  return mCastleRights[color];
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getPieces(Piece type) const
{
  return mPieces[ColorWhite][type] | mPieces[ColorBlack][type];
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getPieces(Color color, Piece type) const
{
  return mPieces[color][type];
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getAllPieces(Color color) const
{
  return mAllPieces[color];
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getOccupied() const
{
  return mOccupied;
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getUnoccupied() const
{
  return ~mOccupied;
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getEnPassant() const
{
  return mEnPassant;
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getPossibleMoves(Piece type, Color color, Square fromSquare) const
{
  switch (type) {
    case PiecePawn:
      if (color == ColorWhite)
        return getWhitePawnAttacksForSquare(fromSquare);
      else
        return getBlackPawnAttacksForSquare(fromSquare);
    case PieceKnight:
      return getKnightAttacksForSquare(fromSquare, color);
    case PieceKing:
      return getKingAttacksForSquare(fromSquare, color);
    case PieceRook:
      return getRookAttacksForSquare(fromSquare, color);
    case PieceBishop:
      return getBishopAttacksForSquare(fromSquare, color);
    case PieceQueen:
      return getQueenAttacksForSquare(fromSquare, color);
    case PieceCount:
    default:
      throw std::runtime_error{"Invalid piece type"};
  }
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getKingBlockers(Color them) const
{
  auto       blockers = Bitboard{};
  auto const us       = ~them;

  auto ksq = getKingSquare(them);

  CHESSGEN_ASSERT(ksq != Square::None);

  auto const rooksOrQueens   = getPieces(us, PieceQueen) | getPieces(us, PieceRook);
  auto const bishopsOrQueens = getPieces(us, PieceQueen) | getPieces(us, PieceBishop);
  auto const rqAttacks = attacks::getSlidingAttacks(PieceRook, ksq, Bitboard{}) & rooksOrQueens;
  auto const bqAttacks = attacks::getSlidingAttacks(PieceBishop, ksq, Bitboard{}) & bishopsOrQueens;

  // Find all sliders aiming towards the king position
  auto sliders = getAllPieces(us) & (rqAttacks | bqAttacks);

  // Mask the sliders out of the occupancy bits
  auto const occupancy = getOccupied() ^ sliders;

  while (sliders) {
    auto const sniperSq = sliders.popLsb();
    auto const b        = Bitboard::getLineBetween(ksq, makeSquare(sniperSq)) & occupancy;

    if (b && !b.moreThanOne()) {
      blockers |= b;
    }
  }
  return blockers;
}
// -------------------------------------------------------------------------------------------------
bool BoardState::isSquareUnderAttack(Color enemy, Square square) const
{
  auto const us = ~enemy;

  auto const pawns = getPossibleMoves(PiecePawn, us, square) & getPieces(enemy, PiecePawn);
  if (pawns) return true;
  auto const knights = getPossibleMoves(PieceKnight, us, square) & getPieces(enemy, PieceKnight);
  if (knights) return true;
  auto const bishops = getPossibleMoves(PieceBishop, us, square) & getPieces(enemy, PieceBishop);
  if (bishops) return true;
  auto const rooks = getPossibleMoves(PieceRook, us, square) & getPieces(enemy, PieceRook);
  if (rooks) return true;
  auto const queens = getPossibleMoves(PieceQueen, us, square) & getPieces(enemy, PieceQueen);
  if (queens) return true;
  auto const king = getPossibleMoves(PieceKing, us, square) & getPieces(enemy, PieceKing);

  return !king.isZero();
}
// -------------------------------------------------------------------------------------------------
PieceInfo BoardState::getPieceOn(Square sq) const
{
  auto const pieces = {
      PieceKing,
      PieceQueen,
      PieceRook,
      PieceBishop,
      PieceKnight,
      PiecePawn,
  };

  for (auto color : {ColorWhite, ColorBlack}) {
    for (auto pt : pieces) {
      if (mPieces[color][pt] & sq) return PieceInfo{pt, color};
    }
  }
  return PieceInfo{PieceNone, ColorNone};
}
// -------------------------------------------------------------------------------------------------
Color BoardState::getColorOfPieceOn(Square sq) const
{
  if (getAllPieces(ColorWhite) & sq)
    return ColorWhite;
  else
    return ColorBlack;
}
// -------------------------------------------------------------------------------------------------
bool BoardState::isSquareEmpty(Square sq) const
{
  return !(getOccupied() & sq);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getCheckSquares(Color color, Piece piece) const
{
  if (piece == PieceKing) return Bitboard{};

  return getPossibleMoves(piece, color, getKingSquare(~color));
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getCheckers() const
{
  if (!isInCheck()) return Bitboard{};

  auto const us  = getActivePlayer();
  auto const ksq = getKingSquare(us);

  auto const pawns   = getPossibleMoves(PiecePawn, us, ksq) & getPieces(~us, PiecePawn);
  auto const knights = getPossibleMoves(PieceKnight, us, ksq) & getPieces(~us, PieceKnight);
  auto const bishops = getPossibleMoves(PieceBishop, us, ksq) & getPieces(~us, PieceBishop);
  auto const rooks   = getPossibleMoves(PieceRook, us, ksq) & getPieces(~us, PieceRook);
  auto const queens  = getPossibleMoves(PieceQueen, us, ksq) & getPieces(~us, PieceQueen);

  return pawns | knights | bishops | rooks | queens;
}
// -------------------------------------------------------------------------------------------------
Square BoardState::getKingSquare(Color color) const
{
  return makeSquare(getPieces(color, PieceKing).lsb());
}
// -------------------------------------------------------------------------------------------------
Square BoardState::getCastlingRookSquare(Color color, CastleSide side) const
{
  // TODO: Support Chess960
  if (side == CastleSide::King)
    return color == ColorWhite ? Square::H1 : Square::H8;
  else
    return color == ColorWhite ? Square::A1 : Square::A8;
}
// -------------------------------------------------------------------------------------------------
Square BoardState::getEnPassantSquare() const
{
  return makeSquare(mEnPassant.lsb());
}
// -------------------------------------------------------------------------------------------------
bool BoardState::isMoveCheck(UCIMove const& move) const
{
  // TODO: Make this faster!
  // We dont need to create a new board and generate all legal moves for it!

  auto temp = Board(*this);
  if (!temp.makeMove(move)) return false;
  return temp.isInCheck();
}
// -------------------------------------------------------------------------------------------------
bool BoardState::isMoveMate(UCIMove const& move) const
{
  // TODO: Make this faster!
  // We dont need to create a new board and generate all legal moves for it!

  auto temp = Board(*this);
  if (!temp.makeMove(move)) return false;
  return temp.isOver() && temp.getGameOverReason() == GameOverReason::Mate;
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getAttackers(Color color, Square square) const
{
  auto const us   = color;
  auto const them = ~color;

  auto const pawns   = getPossibleMoves(PiecePawn, them, square) & getPieces(us, PiecePawn);
  auto const knights = getPossibleMoves(PieceKnight, them, square) & getPieces(us, PieceKnight);
  auto const bishops = getPossibleMoves(PieceBishop, them, square) & getPieces(us, PieceBishop);
  auto const rooks   = getPossibleMoves(PieceRook, them, square) & getPieces(us, PieceRook);
  auto const queens  = getPossibleMoves(PieceQueen, them, square) & getPieces(us, PieceQueen);
  auto const king    = getPossibleMoves(PieceKing, them, square) & getPieces(us, PieceKing);

  return pawns | knights | bishops | rooks | queens | king;
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getWhitePawnAttacksForSquare(Square square) const
{
  return attacks::getNonSlidingAttacks(PiecePawn, square, ColorWhite);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getBlackPawnAttacksForSquare(Square square) const
{
  return attacks::getNonSlidingAttacks(PiecePawn, square, ColorBlack);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getKingAttacksForSquare(Square square, Color color) const
{
  return attacks::getNonSlidingAttacks(PieceKing, square, color) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getKnightAttacksForSquare(Square square, Color color) const
{
  return attacks::getNonSlidingAttacks(PieceKnight, square, color) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getBishopAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(PieceBishop, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getRookAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(PieceRook, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
Bitboard BoardState::getQueenAttacksForSquare(Square square, Color color) const
{
  return attacks::getSlidingAttacks(PieceQueen, square, getOccupied()) & ~getAllPieces(color);
}
// -------------------------------------------------------------------------------------------------
void BoardState::addPiece(Piece type, Color color, Square square)
{
  mPieces[color][type].setBit(square);
  mAllPieces[color].setBit(square);
  mOccupied.setBit(square);
}
// -------------------------------------------------------------------------------------------------
void BoardState::removePiece(Piece type, Color color, Square square)
{
  mPieces[color][type].clearBit(square);
  mAllPieces[color].clearBit(square);
  mOccupied.clearBit(square);
}
// -------------------------------------------------------------------------------------------------
void BoardState::movePiece(Piece type, Color color, Square from, Square to)
{
  if (type == PieceKing) {
    mCastleRights[color] = CastleSide::None;
  } else if (type == PieceRook) {
    if (color == ColorWhite) {
      if (from == Square::A1)
        mCastleRights[color] = mCastleRights[color] & ~CastleSide::Queen;
      else if (from == Square::H1)
        mCastleRights[color] = mCastleRights[color] & ~CastleSide::King;
    } else {
      if (from == Square::A8)
        mCastleRights[color] = mCastleRights[color] & ~CastleSide::Queen;
      else if (from == Square::H8)
        mCastleRights[color] = mCastleRights[color] & ~CastleSide::King;
    }
  } else if (type == PiecePawn) {
    auto const indexFrom = int(from);
    auto const indexTo   = int(to);

    // If it's a double move, see if there is a pawn ready to take en passant
    // if so, record this square
    if (std::abs(indexFrom - indexTo) == 16) {
      auto const pawns = getPieces(~color, PiecePawn);
      auto const toSq  = makeSquare(indexTo);
      auto const epSq  = (indexTo + (color == ColorWhite ? -8 : 8));
      if (getFile(toSq) == File::FileA) {
        if (pawns & (toSq + Direction::East)) {
          mEnPassant.setBit(epSq);
        }
      } else if (getFile(toSq) == File::FileH) {
        if (pawns & (toSq + Direction::West)) {
          mEnPassant.setBit(epSq);
        }
      } else {
        if (pawns & (toSq + Direction::West) || pawns & (toSq + Direction::East)) {
          mEnPassant.setBit(epSq);
        }
      }
    }
    // Reset fifty-rule clock on pawn moves
    mHalfMoves = 0;
  }

  removePiece(type, color, from);
  addPiece(type, color, to);

  updateNonPieceBitboards();
}
// -------------------------------------------------------------------------------------------------
bool BoardState::makeMove(UCIMove const& move)
{
  auto const us     = getActivePlayer();
  auto const behind = us == ColorWhite ? Direction::South : Direction::North;
  auto const them   = ~us;
  auto const from   = move.fromSquare();
  auto const to     = move.toSquare();

  if (!move.isCastling()) {
    CHESSGEN_ASSERT(from != Square::None);
    CHESSGEN_ASSERT(to != Square::None);
    CHESSGEN_ASSERT(getColorOfPieceOn(from) == us);
    CHESSGEN_ASSERT(isSquareEmpty(to) || getColorOfPieceOn(to) == them);
  }

  clearEnPassant();

  if (move.isCastling()) {
    auto const kingside = move.getCastleSide() == CastleSide::King;
    auto const kingFrom = getKingSquare(us);
    auto const rookFrom = getCastlingRookSquare(us, move.getCastleSide());

    auto const rookTo = [&] {
      if (us == ColorWhite)
        return kingside ? Square::F1 : Square::D1;
      else
        return kingside ? Square::F8 : Square::D8;
    }();

    auto const kingTo = [&] {
      if (us == ColorWhite)
        return kingside ? Square::G1 : Square::C1;
      else
        return kingside ? Square::G8 : Square::C8;
    }();

    removePiece(PieceRook, us, rookFrom);
    removePiece(PieceKing, us, kingFrom);
    addPiece(PieceRook, us, rookTo);
    addPiece(PieceKing, us, kingTo);

    mCastleRights[us] = CastleSide::None;
  } else {
    auto captured = move.isEnPassant() ? PiecePawn : getPieceOn(to).type;

    if (captured != PieceNone) {
      mHalfMoves = 0;  // Reset fifty-move counter on captures
      if (move.isEnPassant())
        removePiece(captured, them, to + behind);
      else
        removePiece(captured, them, to);
    }

    movePiece(getPieceOn(from).type, us, from, to);
  }

  if (move.isPromotion()) {
    removePiece(PiecePawn, us, to);
    addPiece(move.promotedTo(), us, to);
  }

  if (us == ColorBlack) {
    ++mFullMove;
  }

  mTurn = ~mTurn;

  return true;
}
}  // namespace chessgen
