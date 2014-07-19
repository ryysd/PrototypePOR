#include "./action_test.h"

class StateTest : public ActionTest {};

TEST_F(StateTest, AfterWordTest) {
  const State* state = philosopher_2_state_space_->init_state();
  Word glf1_grf1_plf1{get_left_fork1_, get_right_fork1_, put_left_fork1_};
  Word glf1_grf1_plf1_prf2{get_left_fork1_, get_right_fork1_, put_left_fork1_, put_left_fork2_};

  EXPECT_EQ("[0,0,1,0,1,0,0,1,0,0]", state->After(glf1_grf1_plf1)->hash());
  EXPECT_EQ(NULL, state->After(glf1_grf1_plf1_prf2));
}
