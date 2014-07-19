#ifndef TEST_ACTION_TEST_H_
#define TEST_ACTION_TEST_H_

#include "./por_test.h"

class ActionTest : public PorTest {
 protected:
  virtual void SetUp() {
    PorTest::SetUp();

    // get_left_fork1_  = philosopher_2_action_table_->FindByName("glf1");
    // get_left_fork2_  = philosopher_2_action_table_->FindByName("glf2");
    // get_right_fork1_ = philosopher_2_action_table_->FindByName("grf1");
    // get_right_fork2_ = philosopher_2_action_table_->FindByName("grf2");

    // put_left_fork1_  = philosopher_2_action_table_->FindByName("plf1");
    // put_left_fork2_  = philosopher_2_action_table_->FindByName("plf2");
    // put_right_fork1_ = philosopher_2_action_table_->FindByName("prf1");
    // put_right_fork2_ = philosopher_2_action_table_->FindByName("prf2");

    get_left_fork1_  = philosopher_2_action_table_->FindByName("glf1_1");
    get_left_fork2_  = philosopher_2_action_table_->FindByName("glf2_1");
    get_right_fork1_ = philosopher_2_action_table_->FindByName("grf1_1");
    get_right_fork2_ = philosopher_2_action_table_->FindByName("grf2_1");

    put_left_fork1_  = philosopher_2_action_table_->FindByName("plf1_1");
    put_left_fork2_  = philosopher_2_action_table_->FindByName("plf2_1");
    put_right_fork1_ = philosopher_2_action_table_->FindByName("prf1_1");
    put_right_fork2_ = philosopher_2_action_table_->FindByName("prf2_1");

    // a1_ = interleave_3_action_table_->FindByName("a1");
    // a2_ = interleave_3_action_table_->FindByName("a2");
    // b1_ = interleave_3_action_table_->FindByName("b1");
    // b2_ = interleave_3_action_table_->FindByName("b2");
    // c1_ = interleave_3_action_table_->FindByName("c1");
    // c2_ = interleave_3_action_table_->FindByName("c2");

    a1_ = interleave_3_action_table_->FindByName("a1_1");
    a2_ = interleave_3_action_table_->FindByName("a2_1");
    b1_ = interleave_3_action_table_->FindByName("b1_1");
    b2_ = interleave_3_action_table_->FindByName("b2_1");
    c1_ = interleave_3_action_table_->FindByName("c1_1");
    c2_ = interleave_3_action_table_->FindByName("c2_1");
  }

  Action* get_left_fork1_, *get_left_fork2_, *get_right_fork1_, *get_right_fork2_;
  Action* put_left_fork1_, *put_left_fork2_, *put_right_fork1_, *put_right_fork2_;
  Action* a1_, *a2_, *b1_, *b2_, *c1_, *c2_;
};

#endif  // TEST_ACTION_TEST_H_
