#ifndef TEST_ACTION_TEST_H_
#define TEST_ACTION_TEST_H_

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

#endif  // TEST_ACTION_TEST_H_
