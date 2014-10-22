#ifndef PROBE_REDUCER_TEST_H_
#define PROBE_REDUCER_TEST_H_

#include "./action_test.h"
#include "../src/por_module/reducer/probe_reducer.h"

// TODO(ryysd) test: all actions are explored

class ProbeReducerTest : public ActionTest {
 public:
  virtual void SetUp() {
    ActionTest::SetUp();
    reducer_ = new ProbeReducer(philosopher_2_action_table_);
  }

 protected:
  ProbeReducer* reducer_;
};

// private method test
// TEST_F(ProbeReducerTest, CalcProbeSetTest) {
//   Word empty{};
//   Word glf1{get_left_fork1_};
// 
//   Vector s0_empty(philosopher_2_state_space_->init_state(), &empty);
//   Vector s0_glf1(philosopher_2_state_space_->init_state(), &glf1);
// 
//   ProbeReducer::ProbeSet probe_set;
//   reducer_->CalcProbeSet(&s0_empty, &probe_set);
//   EXPECT_EQ(1, probe_set.size());
//   EXPECT_EQ("glf1_1", probe_set.begin()->first->name());
// 
//   reducer_->CalcProbeSet(&s0_glf1, &probe_set);
//   EXPECT_EQ(2, probe_set.size());
//   auto it = probe_set.begin();
//   EXPECT_EQ("glf2_1", it->first->name());
//   EXPECT_EQ("grf1_1", (++it)->first->name());
// }

TEST_F(ProbeReducerTest, ReduceTest) {
  // reducer_->Reduce(philosopher_2_state_space_->init_state());
}

TEST_F(ProbeReducerTest, PotentiallyMissedActionTest) {
  MissedAction missed_actions;
  Word glf1_glf2{get_left_fork1_, get_left_fork2_};
  Vector s0_glf1_glf2(philosopher_2_state_space_->init_state(), &glf1_glf2);

  reducer_->CalcPotentiallyMissedAction(&s0_glf1_glf2, &missed_actions);
  EXPECT_EQ(1U, missed_actions.size());
  // EXPECT_EQ("glf2_1", missed_actions.front().first->name());
  // EXPECT_EQ("grf2_1", missed_actions.front().second->name());
}

#endif  // PROBE_REDUCER_TEST_H_

