#include "gtest/gtest.h"

#include "test_fen.hpp"
#include "test_movegen.hpp"

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
