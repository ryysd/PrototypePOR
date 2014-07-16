#ifndef TRANSITION_SYSTEM_WORD_H_
#define TRANSITION_SYSTEM_WORD_H_

#include <vector>
#include <string>
#include <sstream>
#include <iterator>

class Word {
 public:
  Word() : name_(MakeName()) {}
  Word(std::initializer_list<Action*> actions) : actions_(actions), name_(MakeName()) {}
  explicit Word(std::vector<Action*>& actions) : actions_(actions), name_(MakeName()) {}

  const std::vector<Action*>::const_iterator begin() const { return actions_.begin(); }
  const std::vector<Action*>::const_iterator end() const { return actions_.end(); }

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
