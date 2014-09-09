#include "./action_test.h"

class AgeTest : public ActionTest {};

TEST_F(AgeTest, AddTsst) {
  std::vector<const Action*> actions1{a1_, a2_};
  std::vector<const Action*> actions2{b1_, b2_};
  std::vector<const Action*> actions3{c1_, c2_};
  Age age;

  age.Add(actions1);
  EXPECT_EQ(0, age.age(a1_));
  EXPECT_EQ(0, age.age(a2_));

  age.Add(actions2);
  EXPECT_EQ(1, age.age(a1_));
  EXPECT_EQ(1, age.age(a2_));
  EXPECT_EQ(0, age.age(b1_));
  EXPECT_EQ(0, age.age(b2_));

  age.Add(actions3);
  EXPECT_EQ(2, age.age(a1_));
  EXPECT_EQ(2, age.age(a2_));
  EXPECT_EQ(1, age.age(b1_));
  EXPECT_EQ(1, age.age(b2_));
  EXPECT_EQ(0, age.age(c1_));
  EXPECT_EQ(0, age.age(c2_));
}

TEST_F(AgeTest, RemoveTsst) {
  std::vector<const Action*> actions1{a1_, a2_, b1_, b2_, c1_, c2_};
  std::vector<const Action*> actions2{a1_, b1_, c1_};
  Age age;

  age.Add(actions1);
  age.Remove(actions2);

  auto action_ages = age.action_ages();
  EXPECT_EQ(action_ages.end(), action_ages.find(a1_));
  EXPECT_EQ(action_ages.end(), action_ages.find(b1_));
  EXPECT_EQ(action_ages.end(), action_ages.find(c1_));

  EXPECT_NE(action_ages.end(), action_ages.find(a2_));
  EXPECT_NE(action_ages.end(), action_ages.find(b2_));
  EXPECT_NE(action_ages.end(), action_ages.find(c2_));
}

TEST_F(AgeTest, MaxTsst) {
  std::vector<const Action*> actions1{a1_, a2_};
  std::vector<const Action*> actions2{b1_, b2_};
  std::vector<const Action*> actions3{c1_, c2_};
  Age age;

  age.Add(actions1);
  age.Add(actions2);
  age.Add(actions3);

  std::vector<const Action*> max;
  age.Max(&max);

  EXPECT_EQ(2, max.size());
  EXPECT_NE("a1_", max[0]->name());
  EXPECT_NE("a2_", max[0]->name());
}

TEST_F(AgeTest, IsSatisfiedByTsst) {
  std::vector<const Action*> actions1{a1_, a2_};
  std::vector<const Action*> actions2{b1_, b2_};
  std::vector<const Action*> actions3{c1_, c2_};

  std::vector<const Action*> satisfied_actions{a1_, b1_};
  std::vector<const Action*> unsatisfied_actions{b1_, c1_};
  Age age;

  EXPECT_TRUE(age.IsSatisfiedBy(satisfied_actions));
  EXPECT_TRUE(age.IsSatisfiedBy(unsatisfied_actions));

  age.Add(actions1);
  age.Add(actions2);
  age.Add(actions3);

  EXPECT_TRUE(age.IsSatisfiedBy(satisfied_actions));
  EXPECT_FALSE(age.IsSatisfiedBy(unsatisfied_actions));
}
