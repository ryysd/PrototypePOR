#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>
#include <stack>
#include <algorithm>

class Word {
 public:
  Word() : name_(MakeName()) {}
  Word(std::initializer_list<Action*> actions) : actions_(actions), name_(MakeName()) {}
  explicit Word(std::vector<Action*>& actions) : actions_(actions), name_(MakeName()) {}

  const std::vector<Action*>::const_iterator begin() const { return actions_.begin(); }
  const std::vector<Action*>::const_iterator end() const { return actions_.end(); }

  bool WeakEquals(const Word& other) const {
    if (size() != other.size()) return false;

    std::vector<Action*> difference;
    std::set_difference(begin(), end(), other.begin(), other.end(), back_inserter(difference), [](Action* a, Action* b) {return a->Equals(b);});
    if (!difference.empty()) return false;

    std::unique_ptr<Word> sorted = TopologicalSort();
    std::unique_ptr<Word> other_sorted = TopologicalSort();

    return sorted->name() == other_sorted->name();
  }

  std::unique_ptr<Word> TopologicalSort() const {
    std::vector<bool> visited(actions_.size());
    std::vector<int> range(actions_.size());
    std::vector<Action*> sorted;
    std::stack<int> stack;

    std::fill(visited.begin(), visited.end(), false);
    std::iota(range.begin(), range.end(), 0);
    std::sort(range.begin(), range.end(), [this](int i, int j) {return actions_[i]->name() < actions_[j]->name();});

    for (int i : range) {
      if (!visited[i]) stack.push(i);
      while (!stack.empty()) {
        int index = stack.top();

        // postorder
        if (visited[index]) {
          sorted.push_back(actions_[index]);
          stack.pop();
          continue;
        }

        visited[index] = true;

        for (int j = 0; j < index; ++j) {
          if (!visited[j] && (actions_[j]->Influences(actions_[index]) || actions_[index]->Influences(actions_[j]))) {
            stack.push(j);
          }
        }
      }
    }

    return std::unique_ptr<Word>(new Word(sorted));
  }

  Action* operator[](int index) const { return actions_[index]; }
  size_t size() const { return actions_.size(); }

  const std::vector<Action*>& actions() const { return actions_; }
  const std::string& name() const { return name_; }

 private:
  const std::string MakeName() const {
    std::string name = "";
    for (Action* action : actions_) { name += action->name() + ","; }
    if (!name.empty()) name.erase(--name.end());

    return name = "[" + name + "]";
  }

  const std::vector<Action*> actions_;
  const std::string name_;

  DISALLOW_COPY_AND_ASSIGN(Word);
};

#endif  // TRANSITION_SYSTEM_WORD_H_
