#ifndef TEST_POR_TEST_H_
#define TEST_POR_TEST_H_

#include "gtest/gtest.h"
#include "../src/ats_file_reader.h"

class PorTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto pair = ATSFileReader::Read("../c/test/test_data/Philosopher_02.ats.json");
    state_space_ = pair.first;
    action_table_ = pair.second;
  }

  StateSpace* state_space_;
  ActionTable* action_table_;
};

#endif  // TEST_POR_TEST_H_
