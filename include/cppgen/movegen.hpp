#pragma once

#include <vector>
#include "move.hpp"

namespace cppgen
{
class Board;

enum class GenType {
  Quiets,       // Generates all pseudo-legal non-captures and underpromotions
  QuietChecks,  // Generates all pseudo-legal non-captures and underpromotions that give check
  Captures,     // Generates all pseudo-legal captures and queen promotions
  NonEvasions,  // Generates all pseudo-legal captures and non-captures
  Evasions,     // Generates all pseudo-legal moves that get out of check
  Legal,        // Generates all legal moves
};
/**
 * @brief Generate a set of moves based on the given board position
 *
 * @param   board The board to generate moves for
 *
 * @returns The move list
 */
template <GenType Type>
auto generateMoves(Board const& board) -> std::vector<Move>;

}  // namespace cppgen
