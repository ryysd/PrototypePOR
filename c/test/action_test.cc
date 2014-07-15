#include "gtest/gtest.h"
#include "./action_test.h"

TEST_F(ActionTest, SimulateTsst) {
  EXPECT_TRUE(get_left_fork1_->Simulates(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->Simulates(get_right_fork2_));
}

TEST_F(ActionTest, DisableTsst) {
  EXPECT_TRUE(get_left_fork1_->Disables(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->Disables(get_left_fork1_));
}

TEST_F(ActionTest, InfluenceTest) {
  EXPECT_TRUE(get_left_fork1_->Influences(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->Influences(get_right_fork2_));

  EXPECT_TRUE(get_left_fork1_->Influences(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->Influences(get_left_fork1_));
}
