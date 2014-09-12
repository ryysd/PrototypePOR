#ifndef TEST_STATE_SPACE_TEST_H_
#define TEST_STATE_SPACE_TEST_H_

#include "gtest/gtest.h"
#include "./por_test.h"
#include "../src/por_module/ats_file_reader.h"

class StateSpaceTest : public PorTest {};

TEST_F(StateSpaceTest, SizeTest) {
  auto fms_2_data = ATSFileReader::Read("../c/test/test_data/FMS-2.ats.json");
  auto sample_data = ATSFileReader::Read("../c/test/test_data/sample.ats.json");
  auto philosopher_5_data = ATSFileReader::Read("../c/test/test_data/Philosopher_05.ats.json");

  EXPECT_EQ(8, philosopher_2_state_space_->states().size());
  EXPECT_EQ(27, interleave_3_state_space_->states().size());
  EXPECT_EQ(100, sample_data.first->states().size());
  EXPECT_EQ(243, philosopher_5_data.first->states().size());
  EXPECT_EQ(3444, fms_2_data.first->states().size());
}

#endif  // TEST_STATE_SPACE_TEST_H_
