#ifndef TRANSITION_SYSTEM_AGE_H_
#define TRANSITION_SYSTEM_AGE_H_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

class Age {
 public:
  Age() : action_ages_([](const Action* a, const Action* b){ return a->name() < b->name(); }) {};  //NOLINT

  void Add(const std::vector<const Action*> actions) {
    for (auto& kv : action_ages_) { ++(action_ages_[kv.first]); }

    for (const Action* action : actions) {
      if (action_ages_.find(action) == action_ages_.end()) action_ages_.insert(std::make_pair(action, 0));
    }
  }

  void Remove(const std::vector<const Action*> actions) {
    for (const Action* action : actions) action_ages_.erase(action);
  }

  void Max(std::vector<const Action*>* results) const {
    auto comp = [](const action_ages_pair& a, const action_ages_pair& b) { return a.second < b.second; };
    auto max_element = *std::max_element(action_ages_.begin(), action_ages_.end(), comp);
    int max_age = max_element.second;

    for (auto kv : action_ages_) { if (kv.second == max_age) results->push_back(kv.first); }
  }

  // for debug
  bool IsSatisfiedBy(const std::vector<const Action*>& actions) const {
    if (action_ages_.empty()) return true;

    std::vector<const Action*> max, actions_clone(actions.begin(), actions.end());
    Max(&max);

    auto comp = [](const Action* a, const Action* b) { return a->name() < b->name(); };
    std::sort(max.begin(), max.end(), comp);
    std::sort(actions_clone.begin(), actions_clone.end(), comp);

    std::vector<const Action*> intersection;
    std::set_intersection(actions_clone.begin(), actions_clone.end(), max.begin(), max.end(), std::back_inserter(intersection));

    return !intersection.empty();
  }

  int age(const Action* action) const { return action_ages_.at(action); }
  const std::map<const Action*, int, std::function<bool(const Action*, const Action*)>> action_ages() const { return action_ages_; }

 private:
  typedef std::pair<const Action*, int> action_ages_pair;
  std::map<const Action*, int, std::function<bool(const Action*, const Action*)>> action_ages_;
};

#endif  // TRANSITION_SYSTEM_AGE_H_
