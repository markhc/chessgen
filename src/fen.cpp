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

#include "chessgen/fen.hpp"

#include <charconv>

#include "chessgen/helpers.hpp"
#include "chessgen/move.hpp"

namespace chessgen
{
namespace fen
{
// -------------------------------------------------------------------------------------------------
std::array<std::array<Bitboard, 6>, 2> parsePiecePlacement(std::string_view str)
{
  auto pieces   = std::array<std::array<Bitboard, 6>, 2>{};
  auto boardPos = 56ULL;
  for (auto&& currChar : str) {

    if (currChar != '/') {
      if (boardPos >= 64) {
        throw InvalidFenException{"Malformed FEN string"};
      }
    } else {
      if (boardPos > 64) {
        throw InvalidFenException{"Malformed FEN string"};
      }
    }

    switch (currChar) {
      case 'p':
        pieces[Color::Black][Piece::Pawn].setBit(boardPos++);
        break;
      case 'r':
        pieces[Color::Black][Piece::Rook].setBit(boardPos++);
        break;
      case 'n':
        pieces[Color::Black][Piece::Knight].setBit(boardPos++);
        break;
      case 'b':
        pieces[Color::Black][Piece::Bishop].setBit(boardPos++);
        break;
      case 'q':
        pieces[Color::Black][Piece::Queen].setBit(boardPos++);
        break;
      case 'k':
        pieces[Color::Black][Piece::King].setBit(boardPos++);
        break;
      case 'P':
        pieces[Color::White][Piece::Pawn].setBit(boardPos++);
        break;
      case 'R':
        pieces[Color::White][Piece::Rook].setBit(boardPos++);
        break;
      case 'N':
        pieces[Color::White][Piece::Knight].setBit(boardPos++);
        break;
      case 'B':
        pieces[Color::White][Piece::Bishop].setBit(boardPos++);
        break;
      case 'Q':
        pieces[Color::White][Piece::Queen].setBit(boardPos++);
        break;
      case 'K':
        pieces[Color::White][Piece::King].setBit(boardPos++);
        break;
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
        throw InvalidFenException{"Malformed FEN string"};
    }
  }

  if (boardPos != 8) {
    throw InvalidFenException{"Malformed FEN string"};
  }

  return pieces;
}
// -------------------------------------------------------------------------------------------------
Color parsePlayerTurn(std::string_view str)
{
  if (str == "w")
    return Color::White;
  else if (str == "b")
    return Color::Black;
  else
    throw InvalidFenException{"Invalid play turn"};
}
// -------------------------------------------------------------------------------------------------
std::array<CastleSide, 2> parseCastlingAvailability(std::string_view str)
{
  auto castleRights = std::array<CastleSide, 2>{};

  castleRights[Color::White] = CastleSide::None;
  castleRights[Color::Black] = CastleSide::None;

  if (str != "-") {
    for (auto&& c : str) {
      if (c == 'K') {
        castleRights[Color::White] = castleRights[Color::White] | CastleSide::King;
      } else if (c == 'Q') {
        castleRights[Color::White] = castleRights[Color::White] | CastleSide::Queen;
      } else if (c == 'k') {
        castleRights[Color::Black] = castleRights[Color::Black] | CastleSide::King;
      } else if (c == 'q') {
        castleRights[Color::Black] = castleRights[Color::Black] | CastleSide::Queen;
      } else {
        throw InvalidFenException{"Invalid castling rights"};
      }
    }
  }

  return castleRights;
};
// -------------------------------------------------------------------------------------------------
auto parseEnPassantSquare(std::string_view str)
{
  Bitboard enpassant;
  if (str != "-") {
    enpassant.setBit(Move::notationToIndex(str));
  }
  return enpassant;
}
// -------------------------------------------------------------------------------------------------
auto parseHalfMoveNumber(std::string_view str)
{
  auto halfmoves = 0;
  auto [p, ec]   = std::from_chars(str.data(), str.data() + str.size(), halfmoves);
  if (ec != std::errc()) {
    throw InvalidFenException{"Invalid half move value"};
  }
  return halfmoves;
}
// -------------------------------------------------------------------------------------------------
auto parseFullMoveNumber(std::string_view str)
{
  auto fullmoves = 0;
  auto [p, ec]   = std::from_chars(str.data(), str.data() + str.size(), fullmoves);
  if (ec != std::errc()) {
    throw InvalidFenException{"Invalid full move value"};
  }
  return fullmoves;
}
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
GameState toGameState(std::string_view fen)
{
  auto const fields = stringSplit(fen, ' ');

  GameState state;

  if (fields.size() == 6) {
    state.mPieces       = parsePiecePlacement(fields[0]);
    state.mTurn         = parsePlayerTurn(fields[1]);
    state.mCastleRights = parseCastlingAvailability(fields[2]);
    state.mEnPassant    = parseEnPassantSquare(fields[3]);
    state.mHalfMoves    = parseHalfMoveNumber(fields[4]);
    state.mFullMove     = parseFullMoveNumber(fields[5]);
  } else if (fields.size() == 4) {
    state.mPieces       = parsePiecePlacement(fields[0]);
    state.mTurn         = parsePlayerTurn(fields[1]);
    state.mCastleRights = parseCastlingAvailability(fields[2]);
    state.mEnPassant    = parseEnPassantSquare(fields[3]);
    state.mHalfMoves    = 0;
    state.mFullMove     = 1;
  } else {
    throw InvalidFenException{"Malformed FEN string"};
  }

  return state;
}
// -------------------------------------------------------------------------------------------------
std::string fromGameState(GameState const& state)
{
  std::string fen;

  auto getPieceOnSquare = [&](Square square) {
    auto const pieces = {
        Piece::King,
        Piece::Queen,
        Piece::Rook,
        Piece::Bishop,
        Piece::Knight,
        Piece::Pawn,
    };
    for (auto color : {Color::White, Color::Black}) {
      for (auto piece : pieces) {
        if (state.mPieces[color][piece] & square) {
          return std::make_pair(color, piece);
        }
      }
    }
    return std::make_pair(Color::White, Piece::None);
  };

  int emptyCount = 0;

  for (auto i = 56; i >= 0; i -= 8) {
    for (int boardPos = i; boardPos < i + 8; boardPos++) {
      auto const square         = makeSquare(boardPos);
      auto const squareOccupied = !!(state.mOccupied & square);
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

  fen += state.mTurn == Color::White ? " w " : " b ";
  if (state.mCastleRights[Color::White] == CastleSide::None &&
      state.mCastleRights[Color::Black] == CastleSide::None) {
    fen += '-';
  } else {
    if (enumHasFlag(state.mCastleRights[Color::White], CastleSide::King)) fen += 'K';
    if (enumHasFlag(state.mCastleRights[Color::White], CastleSide::Queen)) fen += 'Q';
    if (enumHasFlag(state.mCastleRights[Color::Black], CastleSide::King)) fen += 'k';
    if (enumHasFlag(state.mCastleRights[Color::Black], CastleSide::Queen)) fen += 'q';
  }

  fen += ' ';
  fen += !state.mEnPassant ? "-" : Move::indexToNotation(state.mEnPassant.lsb());
  fen += " " + std::to_string(state.mHalfMoves);
  fen += " " + std::to_string(state.mFullMove);

  return fen;
}
// -------------------------------------------------------------------------------------------------
}  // namespace fen
}  // namespace chessgen
