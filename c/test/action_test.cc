#include <memory>
#include "gtest/gtest.h"
#include "./action_test.h"

TEST_F(ActionTest, SimulatesTsst) {
  EXPECT_TRUE(get_left_fork1_->Simulates(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->Simulates(get_right_fork2_));
}

TEST_F(ActionTest, DisablesTsst) {
  EXPECT_TRUE(get_left_fork1_->Disables(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->Disables(get_left_fork1_));
}

TEST_F(ActionTest, InfluencesTest) {
  EXPECT_TRUE(get_left_fork1_->Influences(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->Influences(get_right_fork2_));

  EXPECT_TRUE(get_left_fork1_->Influences(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->Influences(get_left_fork1_));
}

TEST_F(ActionTest, CalcPrimeCauseTest) {
  Word glf1{get_left_fork1_};
  Word glf1_grf1{get_left_fork1_, get_right_fork1_};

  std::unique_ptr<Word> prime_cause1 = get_right_fork1_->CalcPrimeCause(glf1);
  EXPECT_EQ("[glf1]", prime_cause1->name());

  std::unique_ptr<Word> prime_cause2 = put_left_fork1_->CalcPrimeCause(glf1_grf1);
  EXPECT_EQ("[glf1,grf1]", prime_cause2->name());
}

TEST_F(ActionTest, TopologicalSortTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word c1c2a1a2b1b2{c1_, c2_, a1_, a2_, b1_, b2_};
  Word a2a1b1b2c1c2{a2_, a1_, b1_, b2_, c1_, c2_};

  std::unique_ptr<Word> a1a2b1b2c1c2_sorted = a1a2b1b2c1c2.TopologicalSort();
  std::unique_ptr<Word> c1c2a1a2b1b2_sorted = c1c2a1a2b1b2.TopologicalSort();
  std::unique_ptr<Word> a2a1b1b2c1c2_sorted = a2a1b1b2c1c2.TopologicalSort();

  EXPECT_EQ(a1a2b1b2c1c2_sorted->name(), c1c2a1a2b1b2_sorted->name());
  EXPECT_NE(a1a2b1b2c1c2_sorted->name(), a2a1b1b2c1c2_sorted->name());
}
