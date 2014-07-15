#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>

class Word : public std::vector<Action*> {
 public:
  Word(std::initializer_list<Action*> actions) : std::vector<Action*>(actions) {}
};

#endif  // TRANSITION_SYSTEM_WORD_H_
