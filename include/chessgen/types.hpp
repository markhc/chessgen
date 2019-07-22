#pragma once

#include "platform.hpp"

namespace chessgen
{
enum class Direction {
  North,
  South,
  East,
  West,
  NorthEast,
  NorthWest,
  SouthEast,
  SouthWest,
  Count,
};

enum class File : short {
  FileA,
  FileB,
  FileC,
  FileD,
  FileE,
  FileF,
  FileG,
  FileH,

  Count,
  None = Count,
};

enum class Rank : short {
  Rank1,
  Rank2,
  Rank3,
  Rank4,
  Rank5,
  Rank6,
  Rank7,
  Rank8,

  Count,
  None = Count,
};

constexpr File operator++(File& f)
{
  CG_ASSERT(f <= File::FileH);
  return f = File(static_cast<int>(f) + 1);
}
constexpr File operator--(File& f)
{
  CG_ASSERT(f >= File::FileA);
  return f = File(static_cast<int>(f) - 1);
}

constexpr Rank operator++(Rank& r)
{
  CG_ASSERT(r <= Rank::Rank8);
  return r = Rank(static_cast<int>(r) + 1);
}
constexpr Rank operator--(Rank& r)
{
  CG_ASSERT(r >= Rank::Rank1);
  return r = Rank(static_cast<int>(r) - 1);
}

enum class Square : short {
  // clang-format off
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  Count,
  // clang-format on
};
constexpr Square squareFromIndex(int index) { return Square(index); }
constexpr int    indexFromSquare(Square square) { return int(square); }
constexpr Square makeSquare(File f, Rank r)
{
  CG_ASSERT(f >= File::FileA && f <= File::FileH);
  CG_ASSERT(r >= Rank::Rank1 && r <= Rank::Rank8);
  return Square((static_cast<int>(r) << 3) + static_cast<int>(f));
}
constexpr inline File getFile(Square s)
{
  CG_ASSERT(s >= Square::A1 && s <= Square::H8);
  return File(static_cast<int>(s) & 7);
}
constexpr inline Rank getRank(Square s)
{
  CG_ASSERT(s >= Square::A1 && s <= Square::H8);
  return Rank((static_cast<int>(s) >> 3));
}

constexpr Square operator++(Square& s)
{
  CG_ASSERT(s <= Square::H8);
  return s = Square(static_cast<int>(s) + 1);
}
constexpr Square operator--(Square& s)
{
  CG_ASSERT(s >= Square::A1);
  return s = Square(static_cast<int>(s) - 1);
}
constexpr Square operator+(Square s, Direction d)
{
  auto const f = static_cast<int>(getFile(s));
  auto const r = static_cast<int>(getRank(s));

  switch (d) {
    case Direction::North:
      return makeSquare(File(f), Rank(r + 1));
    case Direction::South:
      return makeSquare(File(f), Rank(r - 1));
    case Direction::West:
      return makeSquare(File(f - 1), Rank(r));
    case Direction::East:
      return makeSquare(File(f + 1), Rank(r));
    case Direction::NorthWest:
      return makeSquare(File(f - 1), Rank(r + 1));
    case Direction::NorthEast:
      return makeSquare(File(f + 1), Rank(r + 1));
    case Direction::SouthWest:
      return makeSquare(File(f - 1), Rank(r - 1));
    case Direction::SouthEast:
      return makeSquare(File(f + 1), Rank(r - 1));
    case Direction::Count:
    default:
      return s;
  }
}
constexpr Square operator-(Square s, Direction d)
{
  switch (d) {
    case Direction::North:
      return operator+(s, Direction::South);
    case Direction::South:
      return operator+(s, Direction::North);
    case Direction::West:
      return operator+(s, Direction::East);
    case Direction::East:
      return operator+(s, Direction::West);
    case Direction::NorthWest:
      return operator+(s, Direction::SouthEast);
    case Direction::NorthEast:
      return operator+(s, Direction::SouthWest);
    case Direction::SouthWest:
      return operator+(s, Direction::NorthEast);
    case Direction::SouthEast:
      return operator+(s, Direction::NorthWest);
    case Direction::Count:
    default:
      return s;
  }
}
// Pieces are often used as an index into a piece array
// I don't like using C enums, but C++ scoped enums
// cannot be implicitly converted to integers, which would
// require casts for every access into the arrays
// This is my middle ground solution.
struct PieceEnum {
  enum Piece_ : short {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King,
    Count,
    None = Count,
  };
};
using Piece = PieceEnum::Piece_;

enum Color {
  White,
  Black,
};
constexpr inline Color operator~(Color c)
{
  return c == Color::White ? Color::Black : Color::White;
}
inline std::string to_string(Rank r)
{
  return std::string{static_cast<char>(static_cast<int>(r) + 1 + '0')};
}
inline std::string to_string(File f)
{
  return std::string{static_cast<char>(static_cast<int>(f) + 'a')};
}
inline std::string to_string(Square s) { return to_string(getFile(s)) + to_string(getRank(s)); }
template <Color color = Color::White>
inline std::string to_string(Piece p)
{
  if constexpr (color == Color::White) {
    switch (p) {
      case Piece::Pawn:
        return "P";
      case Piece::Rook:
        return "R";
      case Piece::Bishop:
        return "B";
      case Piece::Knight:
        return "N";
      case Piece::King:
        return "K";
      case Piece::Queen:
        return "Q";
      case Piece::Count:
      default:
        throw std::runtime_error("Invalid piece");
    }
  } else {
    switch (p) {
      case Piece::Pawn:
        return "p";
      case Piece::Rook:
        return "r";
      case Piece::Bishop:
        return "b";
      case Piece::Knight:
        return "n";
      case Piece::King:
        return "k";
      case Piece::Queen:
        return "q";
      case Piece::Count:
      default:
        throw std::runtime_error("Invalid piece");
    }
  }
}

template <class E>
constexpr int to_int(E e)
{
  return static_cast<int>(e);
}
}  // namespace chessgen
