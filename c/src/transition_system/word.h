#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>

class Word : public std::vector<Action*> {
 public:
  Word(std::initializer_list<Action*> actions) : std::vector<Action*>(actions) {}

  // TODO(ryysd) define this method as method of Action class.
  // This method should be defined as method of Action class.
  // Because of circular reference, this method is defined as method of Word class.
  void CalcPrimeCause(const Action* action, Word* prime_cause) const {
    std::vector<bool> rest_flags(size());
    std::fill(rest_flags.begin(), rest_flags.end(), true);

    std::vector<int> range(size());
    std::iota(range.begin(), range.end(), 0);

    Action* tmp = NULL;
    for (int i = 0, n = size(); i < n; ++i) {
      tmp = (*this)[i];

      auto is_necessary = [tmp, i, &rest_flags, this](int j) { return i != j && rest_flags[j] && tmp->Influences((*this)[j]); };
      rest_flags[i] = (tmp->Influences(action)) || std::any_of(range.begin(), range.end(), is_necessary);
    }

    for (int i = 0, n = size(); i < n; ++i) {
      if (rest_flags[i]) prime_cause->push_back((*this)[i]);
    }
  }
};

#endif  // TRANSITION_SYSTEM_WORD_H_
