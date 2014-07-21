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
  Word(std::initializer_list<const Action*> actions) : actions_(actions), name_(MakeName()) {}
  explicit Word(std::vector<const Action*>& actions) : actions_(actions), name_(MakeName()) {}

  const std::vector<const Action*>::const_iterator begin() const { return actions_.begin(); }
  const std::vector<const Action*>::const_iterator end() const { return actions_.end(); }

  bool WeakEquals(const Word& other) const {
    if (size() != other.size()) return false;
    // if (!ContainsSameActions(other)) return false;

    std::unique_ptr<Word> sorted = TopologicalSort();
    std::unique_ptr<Word> other_sorted = other.TopologicalSort();

    return sorted->name() == other_sorted->name();
  }

  bool IsWeakPrefixOf(const Word& other) const {
    if (size() > other.size()) return false;

    return other.WeakEquals(*(other.Complement(*this)));
  }

  std::unique_ptr<Word> Append(const Action* action) const {
    std::vector<const Action*> new_actions = actions_;
    new_actions.push_back(action);
    return std::unique_ptr<Word>(new Word(new_actions));
  }

  // unefficient implementation
  std::unique_ptr<Word> Diff(const Word& other) const {
    assert(other.IsWeakPrefixOf(*this));

    std::vector<const Action*> difference;
    std::copy(begin(), end(), std::back_inserter(difference));
    for (const Action* action : other) {
      difference.erase(std::find_if(difference.begin(), difference.end(), [action](const Action* a) { return a->Equals(action); }));
    }

    return std::unique_ptr<Word>(new Word(difference));
  }

  const Action* operator[](int index) const { return actions_[index]; }
  size_t size() const { return actions_.size(); }

  const std::vector<const Action*>& actions() const { return actions_; }
  const std::string& name() const { return name_; }

 private:
  std::unique_ptr<Word> TopologicalSort() const {
    std::vector<bool> visited(actions_.size());
    std::vector<int> range(actions_.size());
    std::vector<const Action*> sorted;
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
          if (!visited[j] && !actions_[j]->Equals(actions_[index]) && ((actions_[j]->Influences(actions_[index]) || actions_[index]->Influences(actions_[j])))) {
            stack.push(j);
          }
        }
      }
    }

    return std::unique_ptr<Word>(new Word(sorted));
  }

  // bool ContainsSameActions(const Word& other) const {
  //   std::vector<const Action*> clone, other_clone;
  //   std::copy(begin(), end(), back_inserter(clone));
  //   std::copy(other.begin(), other.end(), back_inserter(other_clone));

  //   auto compare = [](const Action* a, const Action* b) { return a->name() < b->name(); };
  //   std::sort(clone.begin(), clone.end(), compare);
  //   std::sort(other_clone.begin(), other_clone.end(), compare);

  //   for (auto it = clone.begin(), end = clone.end(), other_it = other_clone.begin(); it != end; ++it, ++other_it) {
  //     if (!(*it)->Equals(*other_it)) return false;
  //   }

  //   return true;
  // }

  std::unique_ptr<Word> Complement(const Word& other) const {
    assert(size() >= other.size());
    std::vector<const Action*> complemented;

    std::vector<const Action*> clone, other_clone;
    std::copy(begin(), end(), back_inserter(clone));
    std::copy(other.begin(), other.end(), std::back_inserter(other_clone));
    std::copy(other.begin(), other.end(), std::back_inserter(complemented));

    auto compare = [](const Action* a, const Action* b) { return a->name() < b->name(); };
    std::sort(clone.begin(), clone.end(), compare);
    std::sort(other_clone.begin(), other_clone.end(), compare);

    std::vector<const Action*> difference;
    std::set_difference(clone.begin(), clone.end(), other_clone.begin(), other_clone.end(), std::back_inserter(difference), compare);
    for (const Action* action : difference) complemented.push_back(action);

    assert(complemented.size() == size());
    return std::unique_ptr<Word>(new Word(complemented));
  }

  const std::string MakeName() const {
    std::string name = "";
    for (const Action* action : actions_) { name += action->name() + ","; }
    if (!name.empty()) name.erase(--name.end());

    return "[" + name + "]";
  }

  const std::vector<const Action*> actions_;
  const std::string name_;

  DISALLOW_COPY_AND_ASSIGN(Word);
};

typedef std::unique_ptr<Word> WordPtr;

#endif  // TRANSITION_SYSTEM_WORD_H_
