#include "chessgen/attacks.hpp"
#include <iostream>
#include "chessgen/intrinsics.hpp"
#include "chessgen/rays.hpp"

namespace chessgen
{
namespace attacks
{
namespace magics
{
constexpr std::uint64_t rook[64] = {
    0xa8002c000108020ULL,  0x6c00049b0002001ULL,  0x100200010090040ULL,  0x2480041000800801ULL,
    0x280028004000800ULL,  0x900410008040022ULL,  0x280020001001080ULL,  0x2880002041000080ULL,
    0xa000800080400034ULL, 0x4808020004000ULL,    0x2290802004801000ULL, 0x411000d00100020ULL,
    0x402800800040080ULL,  0xb000401004208ULL,    0x2409000100040200ULL, 0x1002100004082ULL,
    0x22878001e24000ULL,   0x1090810021004010ULL, 0x801030040200012ULL,  0x500808008001000ULL,
    0xa08018014000880ULL,  0x8000808004000200ULL, 0x201008080010200ULL,  0x801020000441091ULL,
    0x800080204005ULL,     0x1040200040100048ULL, 0x120200402082ULL,     0xd14880480100080ULL,
    0x12040280080080ULL,   0x100040080020080ULL,  0x9020010080800200ULL, 0x813241200148449ULL,
    0x491604001800080ULL,  0x100401000402001ULL,  0x4820010021001040ULL, 0x400402202000812ULL,
    0x209009005000802ULL,  0x810800601800400ULL,  0x4301083214000150ULL, 0x204026458e001401ULL,
    0x40204000808000ULL,   0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL,
    0x804040008008080ULL,  0x12000810020004ULL,   0x1000100200040208ULL, 0x430000a044020001ULL,
    0x280009023410300ULL,  0xe0100040002240ULL,   0x200100401700ULL,     0x2244100408008080ULL,
    0x8000400801980ULL,    0x2000810040200ULL,    0x8010100228810400ULL, 0x2000009044210200ULL,
    0x4080008040102101ULL, 0x40002080411d01ULL,   0x2005524060000901ULL, 0x502001008400422ULL,
    0x489a000810200402ULL, 0x1004400080a13ULL,    0x4000011008020084ULL, 0x26002114058042ULL};

constexpr std::uint64_t bishop[64] = {
    0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL,
    0x4042004000000ULL,    0x100822020200011ULL,  0xc00444222012000aULL, 0x28808801216001ULL,
    0x400492088408100ULL,  0x201c401040c0084ULL,  0x840800910a0010ULL,   0x82080240060ULL,
    0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL, 0x8144042209100900ULL,
    0x208081020014400ULL,  0x4800201208ca00ULL,   0xf18140408012008ULL,  0x1004002802102001ULL,
    0x841000820080811ULL,  0x40200200a42008ULL,   0x800054042000ULL,     0x88010400410c9000ULL,
    0x520040470104290ULL,  0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL,
    0x941408200c002000ULL, 0x658810000806011ULL,  0x188071040440a00ULL,  0x4800404002011c00ULL,
    0x104442040404200ULL,  0x511080202091021ULL,  0x4022401120400ULL,    0x80c0040400080120ULL,
    0x8040010040820802ULL, 0x480810700020090ULL,  0x102008e00040242ULL,  0x809005202050100ULL,
    0x8002024220104080ULL, 0x431008804142000ULL,  0x19001802081400ULL,   0x200014208040080ULL,
    0x3308082008200100ULL, 0x41010500040c020ULL,  0x4012020c04210308ULL, 0x208220a202004080ULL,
    0x111040120082000ULL,  0x6803040141280a00ULL, 0x2101004202410000ULL, 0x8200000041108022ULL,
    0x21082088000ULL,      0x2410204010040ULL,    0x40100400809000ULL,   0x822088220820214ULL,
    0x40808090012004ULL,   0x910224040218c9ULL,   0x402814422015008ULL,  0x90014004842410ULL,
    0x1000042304105ULL,    0x10008830412a00ULL,   0x2520081090008908ULL, 0x40102000a0a60140ULL,
};

constexpr int rookBits[64] = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11,
                              11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
                              11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
                              11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

constexpr int bishopBits[64] = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7,
                                5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7,
                                7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};
}  // namespace magics

static Bitboard nonSlidingAttacks[2][6][64] = {};

static Bitboard rookTable[64][4096]   = {};
static Bitboard bishopTable[64][1024] = {};
static Bitboard rookMasks[64]         = {};
static Bitboard bishopMasks[64]       = {};
static Bitboard lines[64][64]         = {};

void initPawnAttacks();
void initKnightAttacks();
void initKingAttacks();
void initRookMagicTable();
void initBishopMagicTable();
void initRookMasks();
void initBishopMasks();

Bitboard getRookAttacksSlow(int square, Bitboard blockers);
Bitboard getBishopAttacksSlow(int square, Bitboard blockers);
Bitboard getRookAttacks(int square, Bitboard blockers);
Bitboard getBishopAttacks(int square, Bitboard blockers);
Bitboard getBlockersFromIndex(int index, Bitboard blockerMask);

// -------------------------------------------------------------------------------------------------
void precomputeTables()
{
  initPawnAttacks();
  initKnightAttacks();
  initKingAttacks();

  initRookMasks();
  initBishopMasks();

  initRookMagicTable();
  initBishopMagicTable();

  for (Square s1 = Square::A1; s1 <= Square::H8; ++s1) {
    for (auto&& pt : {Piece::Bishop, Piece::Rook}) {
      for (Square s2 = Square::A1; s2 <= Square::H8; ++s2) {
        if (getSlidingAttacks(pt, s1, 0ULL) & s2) {
          lines[to_int(s1)][to_int(s2)] =
              ((getSlidingAttacks(pt, s1, 0ULL) & getSlidingAttacks(pt, s2, 0ULL)) | s1) | s2;
        }
      }
    }
  }
}
// -------------------------------------------------------------------------------------------------
Bitboard getLineBetween(Square s1, Square s2) { return lines[to_int(s1)][to_int(s2)]; }
// -------------------------------------------------------------------------------------------------
Bitboard getNonSlidingAttacks(Piece piece, Square from, Color color)
{
  return nonSlidingAttacks[color][piece][indexFromSquare(from)];
}
// -------------------------------------------------------------------------------------------------
Bitboard getSlidingAttacks(Piece piece, Square from, Bitboard blockers)
{
  switch (piece) {
    case Piece::Bishop:
      return getBishopAttacks(indexFromSquare(from), blockers);
    case Piece::Rook:
      return getRookAttacks(indexFromSquare(from), blockers);
    case Piece::Queen:
      return getBishopAttacks(indexFromSquare(from), blockers) |
             getRookAttacks(indexFromSquare(from), blockers);
    case Piece::King:
    case Piece::Pawn:
    case Piece::Knight:
    default:
      throw std::runtime_error("Not a sliding piece");
  }
}
// -------------------------------------------------------------------------------------------------
void initPawnAttacks()
{
  for (int i = 0; i < 64; i++) {
    auto const start = 1ULL << i;

    auto const whiteAttackBb = ((start << 9) & ~Bitboards::FileA) | ((start << 7) & ~Bitboards::FileH);
    auto const blackAttackBb = ((start >> 9) & ~Bitboards::FileH) | ((start >> 7) & ~Bitboards::FileA);

    nonSlidingAttacks[Color::White][Piece::Pawn][i] = Bitboard{whiteAttackBb};
    nonSlidingAttacks[Color::Black][Piece::Pawn][i] = Bitboard{blackAttackBb};
  }
}
// -------------------------------------------------------------------------------------------------
void initKnightAttacks()
{
  for (int i = 0; i < 64; i++) {
    auto const start = 1ULL << i;

    auto const attackBb =
        (((start << 15) | (start >> 17)) & ~Bitboards::FileH) |                      // Left 1
        (((start >> 15) | (start << 17)) & ~Bitboards::FileA) |                      // Right 1
        (((start << 6) | (start >> 10)) & ~(Bitboards::FileG | Bitboards::FileH)) |  // Left 2
        (((start >> 6) | (start << 10)) & ~(Bitboards::FileA | Bitboards::FileB));   // Right 2

    nonSlidingAttacks[Color::White][Piece::Knight][i] =
        nonSlidingAttacks[Color::Black][Piece::Knight][i] = Bitboard{attackBb};
  }
}
// -------------------------------------------------------------------------------------------------
void initKingAttacks()
{
  for (int i = 0; i < 64; i++) {
    auto const start = 1ULL << i;

    auto const attackBb = (((start << 7) | (start >> 9) | (start >> 1)) & (~Bitboards::FileH)) |
                          (((start << 9) | (start >> 7) | (start << 1)) & (~Bitboards::FileA)) |
                          ((start >> 8) | (start << 8));

    nonSlidingAttacks[Color::White][Piece::King][i] =
        nonSlidingAttacks[Color::Black][Piece::King][i] = Bitboard{attackBb};
  }
}
// -------------------------------------------------------------------------------------------------
void initRookMagicTable()
{
  // For all squares
  for (auto square = 0; square < 64; ++square) {
    // For all possible blockers for this square
    for (int blockerIndex = 0; blockerIndex < (1 << magics::rookBits[square]); blockerIndex++) {
      auto const blockers = getBlockersFromIndex(blockerIndex, rookMasks[square]);
      auto const hash     = (blockers * magics::rook[square]) >> (64 - magics::rookBits[square]);
      rookTable[square][hash] = getRookAttacksSlow(square, blockers);
    }
  }
}
// -------------------------------------------------------------------------------------------------
void initBishopMagicTable()
{
  // For all squares
  for (auto square = 0; square < 64; ++square) {
    // For all possible blockers for this square
    for (int blockerIndex = 0; blockerIndex < (1 << magics::bishopBits[square]); blockerIndex++) {
      auto const blockers = getBlockersFromIndex(blockerIndex, bishopMasks[square]);
      auto const hash = (blockers * magics::bishop[square]) >> (64 - magics::bishopBits[square]);
      bishopTable[square][hash] = getBishopAttacksSlow(square, blockers);
    }
  }
}
// -------------------------------------------------------------------------------------------------
void initRookMasks()
{
  for (auto square = 0; square < 64; ++square) {
    rookMasks[square] = (rays::getRayForSquare(Direction::North, square) & ~Bitboards::Rank8) |
                        (rays::getRayForSquare(Direction::South, square) & ~Bitboards::Rank1) |
                        (rays::getRayForSquare(Direction::East, square) & ~Bitboards::FileH) |
                        (rays::getRayForSquare(Direction::West, square) & ~Bitboards::FileA);
  }
}
// -------------------------------------------------------------------------------------------------
void initBishopMasks()
{
  auto const edges = Bitboards::FileA | Bitboards::FileH | Bitboards::Rank1 | Bitboards::Rank8;
  for (auto square = 0; square < 64; ++square) {
    bishopMasks[square] = rays::getRayForSquare(Direction::NorthEast, square) |
                          rays::getRayForSquare(Direction::NorthWest, square) |
                          rays::getRayForSquare(Direction::SouthEast, square) |
                          rays::getRayForSquare(Direction::SouthWest, square);

    bishopMasks[square] = bishopMasks[square] & ~edges;
  }
}
// -------------------------------------------------------------------------------------------------
Bitboard getRookAttacksSlow(int square, Bitboard blockers)
{
  std::uint64_t attacks = 0;

  attacks |= rays::getRayForSquare(Direction::North, square);
  if (rays::getRayForSquare(Direction::North, square) & blockers) {
    auto maskedBlockers = rays::getRayForSquare(Direction::North, square) & blockers;
    attacks &= ~(rays::getRayForSquare(Direction::North, bitscan_forward(maskedBlockers)));
  }

  attacks |= rays::getRayForSquare(Direction::South, square);
  if (rays::getRayForSquare(Direction::South, square) & blockers) {
    auto maskedBlockers = rays::getRayForSquare(Direction::South, square) & blockers;
    attacks &= ~(rays::getRayForSquare(Direction::South, bitscan_reverse(maskedBlockers)));
  }

  attacks |= rays::getRayForSquare(Direction::East, square);
  if (rays::getRayForSquare(Direction::East, square) & blockers) {
    auto maskedBlockers = rays::getRayForSquare(Direction::East, square) & blockers;
    attacks &= ~(rays::getRayForSquare(Direction::East, bitscan_forward(maskedBlockers)));
  }

  // Direction::West
  attacks |= rays::getRayForSquare(Direction::West, square);
  if (rays::getRayForSquare(Direction::West, square) & blockers) {
    auto maskedBlockers = rays::getRayForSquare(Direction::South, square) & blockers;
    attacks &= ~(rays::getRayForSquare(Direction::West, bitscan_reverse(maskedBlockers)));
  }

  return Bitboard{attacks};
}
// -------------------------------------------------------------------------------------------------
Bitboard getBishopAttacksSlow(int square, Bitboard blockers)
{
  Bitboard attacks{0};

  // Direction::North Direction::West
  attacks |= rays::getRayForSquare(Direction::NorthWest, square);
  if (rays::getRayForSquare(Direction::NorthWest, square) & blockers) {
    attacks &= ~(rays::getRayForSquare(
        Direction::NorthWest,
        (bitscan_forward(rays::getRayForSquare(Direction::NorthWest, square) & blockers))));
  }

  // Direction::North Direction::East
  attacks |= rays::getRayForSquare(Direction::NorthEast, square);
  if (rays::getRayForSquare(Direction::NorthEast, square) & blockers) {
    attacks &= ~(rays::getRayForSquare(
        Direction::NorthEast,
        bitscan_forward(rays::getRayForSquare(Direction::NorthEast, square) & blockers)));
  }

  // Direction::South Direction::East
  attacks |= rays::getRayForSquare(Direction::SouthEast, square);
  if (rays::getRayForSquare(Direction::SouthEast, square) & blockers) {
    attacks &= ~(rays::getRayForSquare(
        Direction::SouthEast,
        bitscan_reverse(rays::getRayForSquare(Direction::SouthEast, square) & blockers)));
  }

  // Direction::South Direction::West
  attacks |= rays::getRayForSquare(Direction::SouthWest, square);
  if (rays::getRayForSquare(Direction::SouthWest, square) & blockers) {
    attacks &= ~(rays::getRayForSquare(
        Direction::SouthWest,
        bitscan_reverse(rays::getRayForSquare(Direction::SouthWest, square) & blockers)));
  }

  return attacks;
}
// -------------------------------------------------------------------------------------------------
Bitboard getRookAttacks(int square, Bitboard blockers)
{
  blockers &= rookMasks[square];
  auto const key = (blockers * magics::rook[square]) >> (64 - magics::rookBits[square]);
  return rookTable[square][key];
}
// -------------------------------------------------------------------------------------------------
Bitboard getBishopAttacks(int square, Bitboard blockers)
{
  blockers &= bishopMasks[square];
  auto const key = (blockers * magics::bishop[square]) >> (64 - magics::bishopBits[square]);
  int        sq  = square;
  return bishopTable[sq][key];
}
// -------------------------------------------------------------------------------------------------
Bitboard getBlockersFromIndex(int index, Bitboard blockerMask)
{
  auto mask     = blockerMask;
  auto blockers = 0ULL;
  auto bits     = pop_count(mask);
  for (auto i = 0; i < bits; i++) {
    int bitPos = pop_lsb(mask);
    if (index & (1 << i)) {
      blockers |= (1ULL << bitPos);
    }
  }
  return Bitboard{blockers};
}
}  // namespace attacks
}  // namespace chessgen
