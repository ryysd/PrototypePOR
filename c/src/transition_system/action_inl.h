#ifndef TRANSITION_SYSTEM_ACTION_INL_H_
#define TRANSITION_SYSTEM_ACTION_INL_H_

#include <vector>
#include <memory>

inline std::unique_ptr<Word> Action::CalcPrimeCause(const Word& word) const {
  std::vector<bool> rest_flags(word.size());
  std::fill(rest_flags.begin(), rest_flags.end(), true);

  std::vector<int> range(word.size());
  std::iota(range.begin(), range.end(), 0);

  for (int i : range) {
    const Action* action = word[i];

    auto is_necessary = [action, i, &rest_flags, &word](int j) { return i != j && rest_flags[j] && action->Influences(word[j]); };
    rest_flags[i] = action->Influences(this) || std::any_of(range.begin(), range.end(), is_necessary);
  }

  std::vector<const Action*> prime_cause;
  for (int i : range) {
    if (rest_flags[i]) prime_cause.push_back(word[i]);
  }

  return std::unique_ptr<Word>(new Word(prime_cause));
}

#endif  // TRANSITION_SYSTEM_ACTION_INL_H_
