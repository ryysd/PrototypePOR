#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>
#include <string>
#include <sstream>
#include <iterator>

class Word : public std::vector<Action*> {
 public:
  Word() {}
  Word(std::initializer_list<Action*> actions) : std::vector<Action*>(actions) {}

  const std::string& name() { return name_.empty() ? MakeName() : name_; }

 private:
  const std::string& MakeName() {
    for (Action* action : *this) { name_ += action->name() + ","; }
    if (!name_.empty()) name_.erase(--name_.end());

    return name_ = "[" + name_ + "]";
  }

  std::string name_;
};

#endif  // TRANSITION_SYSTEM_WORD_H_
