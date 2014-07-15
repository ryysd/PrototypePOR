#include "gtest/gtest.h"
#include "./por_test.h"

class ActionTest : public PorTest {
 protected:
  virtual void SetUp() {
    PorTest::SetUp();

    get_left_fork1_ = action_table_->FindByName("glf1");
    get_left_fork2_ = action_table_->FindByName("glf2");
    get_right_fork1_ = action_table_->FindByName("grf1");
    get_right_fork2_ = action_table_->FindByName("grf2");

    put_left_fork1_ = action_table_->FindByName("plf1");
    put_left_fork2_ = action_table_->FindByName("plf2");
    put_right_fork1_ = action_table_->FindByName("prf1");
    put_right_fork2_ = action_table_->FindByName("prf2");
  }

  Action* get_left_fork1_, *get_left_fork2_, *get_right_fork1_, *get_right_fork2_;
  Action* put_left_fork1_, *put_left_fork2_, *put_right_fork1_, *put_right_fork2_;
};

TEST_F(ActionTest, SimulateTsst) {
  EXPECT_TRUE(get_left_fork1_->isSimulate(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->isSimulate(get_right_fork2_));
}

TEST_F(ActionTest, DisableTsst) {
  EXPECT_TRUE(get_left_fork1_->isDisable(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->isDisable(get_left_fork1_));
}

TEST_F(ActionTest, InfluenceTest) {
  EXPECT_TRUE(get_left_fork1_->isInfluence(get_right_fork1_));
  EXPECT_TRUE(get_left_fork2_->isInfluence(get_right_fork2_));

  EXPECT_TRUE(get_left_fork1_->isInfluence(get_right_fork2_));
  EXPECT_TRUE(get_right_fork2_->isInfluence(get_left_fork1_));
}
