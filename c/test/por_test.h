#ifndef TEST_POR_TEST_H_
#define TEST_POR_TEST_H_

#include "gtest/gtest.h"
#include "../src/por_module/ats_file_reader.h"

class PorTest : public testing::Test {
 protected:
  virtual void SetUp() {
    auto philosopher_2_pair = ATSFileReader::Read("../c/test/test_data/Philosopher_02.ats.json");
    auto interleave_3_pair = ATSFileReader::Read("../c/test/test_data/interleave_3.ats.json");

    philosopher_2_state_space_ = philosopher_2_pair.first;
    philosopher_2_action_table_ = philosopher_2_pair.second;

    interleave_3_state_space_ = interleave_3_pair.first;
    interleave_3_action_table_ = interleave_3_pair.second;
  }

  StateSpace* philosopher_2_state_space_;
  ActionTable* philosopher_2_action_table_;

  StateSpace* interleave_3_state_space_;
  ActionTable* interleave_3_action_table_;
};

#endif  // TEST_POR_TEST_H_
