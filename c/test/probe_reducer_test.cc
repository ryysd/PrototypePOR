#ifndef PROBE_REDUCER_TEST_H_
#define PROBE_REDUCER_TEST_H_

#include "./action_test.h"
#include "../src/reducer/probe_reducer.h"

class ProbeReducerTest : public ActionTest {
 public:
  virtual void SetUp() {
    ActionTest::SetUp();
    reducer_ = new ProbeReducer(philosopher_2_action_table_);
  }

 protected:
  ProbeReducer* reducer_;
};

TEST_F(ProbeReducerTest, CalcProbeSetTest) {
  Word empty{};
  Word glf1{get_left_fork1_};

  Vector s0_empty(philosopher_2_state_space_->init_state(), &empty);
  Vector s0_glf1(philosopher_2_state_space_->init_state(), &glf1);

  ProbeReducer::ProbeSet probe_set;
  reducer_->CalcProbeSet(&s0_empty, &probe_set);
  EXPECT_EQ(1, probe_set.size());
  EXPECT_EQ("glf1_1", probe_set.begin()->first->name());

  reducer_->CalcProbeSet(&s0_glf1, &probe_set);
  EXPECT_EQ(2, probe_set.size());
  auto it = probe_set.begin();
  EXPECT_EQ("glf2_1", it->first->name());
  EXPECT_EQ("grf1_1", (++it)->first->name());
}

#endif  // PROBE_REDUCER_TEST_H_

