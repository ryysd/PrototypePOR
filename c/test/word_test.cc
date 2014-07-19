#include "./action_test.h"

class WordTest : public ActionTest {};

TEST_F(WordTest, NameTest) {
  Word word{get_left_fork1_, get_right_fork1_, get_left_fork2_, get_right_fork2_};
  EXPECT_EQ("[glf1,grf1,glf2,grf2]", word.name());

  Word empty_word;
  EXPECT_EQ("[]", empty_word.name());
}

TEST_F(WordTest, TopologicalSortTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word c1c2a1a2b1b2{c1_, c2_, a1_, a2_, b1_, b2_};
  Word a2a1b1b2c1c2{a2_, a1_, b1_, b2_, c1_, c2_};

  std::unique_ptr<Word> a1a2b1b2c1c2_sorted = a1a2b1b2c1c2.TopologicalSort();
  std::unique_ptr<Word> c1c2a1a2b1b2_sorted = c1c2a1a2b1b2.TopologicalSort();
  std::unique_ptr<Word> a2a1b1b2c1c2_sorted = a2a1b1b2c1c2.TopologicalSort();

  EXPECT_EQ(a1a2b1b2c1c2_sorted->name(), c1c2a1a2b1b2_sorted->name());
  EXPECT_NE(a1a2b1b2c1c2_sorted->name(), a2a1b1b2c1c2_sorted->name());
}

TEST_F(WordTest, WeakEqualTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word c1c2a1a2b1b2{c1_, c2_, a1_, a2_, b1_, b2_};
  Word a2a1b1b2c1c2{a2_, a1_, b1_, b2_, c1_, c2_};
  Word a1a2b1b2c1{a1_, a2_, b1_, b2_, c1_};
  Word a1a2b1b2c2{a1_, a2_, b1_, b2_, c2_};

  EXPECT_TRUE(a1a2b1b2c1c2.WeakEquals(c1c2a1a2b1b2));
  EXPECT_FALSE(a1a2b1b2c1c2.WeakEquals(a2a1b1b2c1c2));
  EXPECT_FALSE(a1a2b1b2c1c2.WeakEquals(a1a2b1b2c1));
  EXPECT_FALSE(a1a2b1b2c1.WeakEquals(a1a2b1b2c2));
}
