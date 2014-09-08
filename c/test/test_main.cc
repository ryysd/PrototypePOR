#include <iostream>
#include "gtest/gtest.h"
#include "./por_test_environment.h"

GTEST_API_ int main(int argc, char **argv) {
  std::cout << "Running main() from testmain.cc\n";

  testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new PorTestEnvironment);
  return RUN_ALL_TESTS();
}
