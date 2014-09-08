#include <stack>
#include <map>
#include "./action_test.h"

class StateTest : public ActionTest {};

TEST_F(StateTest, AfterWordTest) {
  const State* state = philosopher_2_state_space_->init_state();
  Word glf1_grf1_plf1{get_left_fork1_, get_right_fork1_, put_left_fork1_};
  Word glf1_grf1_plf1_prf2{get_left_fork1_, get_right_fork1_, put_left_fork1_, put_left_fork2_};

  EXPECT_EQ("[@fork1,@thinking2,@tmp1_2,fork1_1,thinking2_1,tmp1_2_1]", state->After(glf1_grf1_plf1)->hash());
  EXPECT_EQ(NULL, state->After(glf1_grf1_plf1_prf2));
}

TEST_F(StateTest, EnableTest) {
  const State* state = philosopher_2_state_space_->init_state();
  EXPECT_TRUE(state->Enables(get_left_fork2_));
  EXPECT_TRUE(state->Enables(get_left_fork1_));

  EXPECT_FALSE(state->Enables(get_right_fork2_));
  EXPECT_FALSE(state->Enables(get_right_fork1_));

  EXPECT_FALSE(state->Enables(put_left_fork2_));
  EXPECT_FALSE(state->Enables(put_left_fork1_));

  EXPECT_FALSE(state->Enables(put_right_fork2_));
  EXPECT_FALSE(state->Enables(put_right_fork1_));
}

TEST_F(StateTest, ExpandTest) {
  const State* state = philosopher_2_state_space_->init_state();
  EntitySet new_entities1;
  state->Expand(get_left_fork2_, &new_entities1);
  State succ1(new_entities1);
  EXPECT_EQ("[@fork1,@thinking1,@tmp2_1,fork1_1,thinking1_1,tmp2_1_1]", succ1.hash());

  EntitySet new_entities2;
  state->Expand(get_left_fork1_, &new_entities2);
  State succ2(new_entities2);
  EXPECT_EQ("[@fork2,@thinking2,@tmp1_1,fork2_1,thinking2_1,tmp1_1_1]", succ2.hash());
}
