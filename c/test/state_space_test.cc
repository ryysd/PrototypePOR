#ifndef TEST_STATE_SPACE_TEST_H_
#define TEST_STATE_SPACE_TEST_H_

#include "gtest/gtest.h"
#include "./por_test.h"

class StateSpaceTest : public PorTest {};

TEST_F(StateSpaceTest, SizeTest) {
  EXPECT_EQ(8, philosopher_2_state_space_->states().size());
  EXPECT_EQ(27, interleave_3_state_space_->states().size());
}

#endif  // TEST_STATE_SPACE_TEST_H_
