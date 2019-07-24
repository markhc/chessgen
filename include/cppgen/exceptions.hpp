#pragma once

#include <fmt/format.h>
#include <stdexcept>
#include <string_view>

namespace cppgen
{
class InvalidSANException : public std::runtime_error
{
public:
  InvalidSANException(std::string_view san)
      : std::runtime_error(fmt::format("\"{}\" is not a valid Standard Algebraic Notation", san))
  {
  }
};
class InvalidFENException : public std::runtime_error
{
public:
  InvalidFENException(std::string_view fen)
      : std::runtime_error(fmt::format("\"{}\" is not a valid FEN", fen))
  {
  }
};
}  // namespace cppgen
