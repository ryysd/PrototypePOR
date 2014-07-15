#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>

class Word {
 public:
  explicit Word(const std::vector<Action*>& actions) : actions_(actions) {}

  const std::vector<Action*>& actionr() const { return actions_; }
 private:
  std::vector<Action*> actions_;
};

#endif  // TRANSITION_SYSTEM_WORD_H_
