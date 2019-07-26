#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>

#include "san_tests.hpp"

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
