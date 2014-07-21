#include "./action_test.h"

class WordTest : public ActionTest {};

TEST_F(WordTest, NameTest) {
  Word word{get_left_fork1_, get_right_fork1_, get_left_fork2_, get_right_fork2_};
  EXPECT_EQ("[glf1_1,grf1_1,glf2_1,grf2_1]", word.name());

  Word empty_word;
  EXPECT_EQ("[]", empty_word.name());
}

// test of private method
// TEST_F(WordTest, TopologicalSortTest) {
//   Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
//   Word c1c2a1a2b1b2{c1_, c2_, a1_, a2_, b1_, b2_};
//   Word a2a1b1b2c1c2{a2_, a1_, b1_, b2_, c1_, c2_};
// 
//   std::unique_ptr<Word> a1a2b1b2c1c2_sorted = a1a2b1b2c1c2.TopologicalSort();
//   std::unique_ptr<Word> c1c2a1a2b1b2_sorted = c1c2a1a2b1b2.TopologicalSort();
//   std::unique_ptr<Word> a2a1b1b2c1c2_sorted = a2a1b1b2c1c2.TopologicalSort();
// 
//   EXPECT_EQ(a1a2b1b2c1c2_sorted->name(), c1c2a1a2b1b2_sorted->name());
//   EXPECT_NE(a1a2b1b2c1c2_sorted->name(), a2a1b1b2c1c2_sorted->name());
// }

TEST_F(WordTest, WeakEqualTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word c1c2a1a2b1b2{c1_, c2_, a1_, a2_, b1_, b2_};
  Word a2a1b1b2c1c2{a2_, a1_, b1_, b2_, c1_, c2_};
  Word a1a2b1b2c1{a1_, a2_, b1_, b2_, c1_};
  Word a1a2b1b2c2{a1_, a2_, b1_, b2_, c2_};

  Word a1b1c1a2c2b2{a1_, b1_, c1_, a2_, c2_, b2_};

  Word a1a2b1b2c1a1a1a2a2{a1_, a2_, b1_, b2_, c1_, a1_, a1_, a2_, a2_};
  Word a1a2b1b2c1a1a2a1a2{a1_, a2_, b1_, b2_, c1_, a1_, a2_, a1_, a2_};

  Word a1a1a2a2{a1_, a1_, a2_, a2_};
  Word a1a2a1a2{a1_, a2_, a1_, a2_};

  EXPECT_TRUE(a1a2b1b2c1c2.WeakEquals(c1c2a1a2b1b2));
  EXPECT_FALSE(a1a2b1b2c1c2.WeakEquals(a2a1b1b2c1c2));
  EXPECT_FALSE(a1a2b1b2c1c2.WeakEquals(a1a2b1b2c1));
  EXPECT_FALSE(a1a2b1b2c1.WeakEquals(a1a2b1b2c2));
  EXPECT_TRUE(a1a2b1b2c1c2.WeakEquals(a1b1c1a2c2b2));

  // These words are invalid in Entity-Based System.
  // So, these words can not exist.
  // EXPECT_TRUE(a1a2b1b2c1a1a1a2a2.WeakEquals(a1a2b1b2c1a1a2a1a2));
  // EXPECT_TRUE(a1a1a2a2.WeakEquals(a1a2a1a2));
}

TEST_F(WordTest, WeakPrefixTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word b1b2a1a2{b1_, b2_, a1_, a2_};
  Word b1b2a2a1{b1_, b2_, a2_, a1_};

  EXPECT_TRUE(b1b2a1a2.IsWeakPrefixOf(a1a2b1b2c1c2));
  EXPECT_FALSE(b1b2a2a1.IsWeakPrefixOf(a1a2b1b2c1c2));
}

TEST_F(WordTest, AppendTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  WordPtr new_word = a1a2b1b2c1c2.Append(a1_);
  EXPECT_EQ("[a1_1,a2_1,b1_1,b2_1,c1_1,c2_1,a1_1]", new_word->name());
}

TEST_F(WordTest, DiffTest) {
  Word a1a2b1b2c1c2{a1_, a2_, b1_, b2_, c1_, c2_};
  Word a1a2b1{a1_, a2_, b1_};
  WordPtr new_word = a1a2b1b2c1c2.Diff(a1a2b1);
  EXPECT_EQ("[b2_1,c1_1,c2_1]", new_word->name());
}
