#ifndef TRANSITION_SYSTEM_ACTION_INL_H_
#define TRANSITION_SYSTEM_ACTION_INL_H_

#include <vector>

inline void Action::CalcPrimeCause(const Word& word, Word* prime_cause) const {
  std::vector<bool> rest_flags(word.size());
  std::fill(rest_flags.begin(), rest_flags.end(), true);

  std::vector<int> range(word.size());
  std::iota(range.begin(), range.end(), 0);

  Action* action = NULL;
  for (int i = 0, n = word.size(); i < n; ++i) {
    action = word[i];

    auto is_necessary = [action, i, &rest_flags, &word](int j) { return i != j && rest_flags[j] && action->Influences(word[j]); };
    rest_flags[i] = (action->Influences(this)) || std::any_of(range.begin(), range.end(), is_necessary);
  }

  for (int i = 0, n = word.size(); i < n; ++i) {
    if (rest_flags[i]) prime_cause->push_back(word[i]);
  }
}

#endif  // TRANSITION_SYSTEM_ACTION_INL_H_
