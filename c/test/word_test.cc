#include "./action_test.h"

class WordTest : public ActionTest {};

TEST_F(WordTest, NameTest) {
  Word word{get_left_fork1_, get_right_fork1_, get_left_fork2_, get_right_fork2_};
  EXPECT_EQ("[glf1,grf1,glf2,grf2]", word.name());

  Word empty_word;
  EXPECT_EQ("[]", empty_word.name());
}
