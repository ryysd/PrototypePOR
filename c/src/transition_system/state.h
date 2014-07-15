#ifndef TRANSITION_SYSTEM_STATE_H_
#define TRANSITION_SYSTEM_STATE_H_

#include <string>
#include <vector>

// TODO(ryysd) use std::array instead of std::vector (if vector is bottleneck of performance)
// class State<int EntitySize, int TransitionSize> {
// State(..., const std::array<std::string, EntitySize> entities) {
// ...
// }
// ...
// }
// create_state(std::vector<std::string> entities) {new State<entities.size()>(...)}

/*! @class State
 *  @brief class representing state
*/
class State {
 public:
  State(const std::string& name, const EntitySet& entities)
    : reduced_(false), name_(name), entities_(entities) {}

  ~State() { for (auto t : transitions_) { delete t; } }

  void AddTransition(const State* target, const Action* action) { transitions_.push_back(new Transition(this, target, action)); }

  const State* After(const Action* action) const {
    if (!action) return NULL;

    auto it = std::find_if(transitions_.begin(), transitions_.end(), [action](const Transition* t) { return t->action()->Equals(action); });
    return it == transitions_.end() ? NULL : (*it)->target();
  }

  const State* After(const Word* word) const {
    if (!word) return NULL;

    const State* s = this;
    for (Action* action : *word) { if (!(s = s->After(action))) return NULL; }
    return s;
  }

  const State* After(const Word& word) const { return After(&word); }

  bool Equals(const State& other) const { return name_ == other.name(); }

  bool reduced() const { return reduced_; }
  const std::string& name() const { return name_; }
  const std::vector<Transition*>& transitions() const { return transitions_; }
  const std::vector<std::string>& entities() const { return entities_; }

 private:
  bool reduced_;
  const std::string name_;
  const std::vector<std::string> entities_;
  std::vector<Transition*> transitions_;

  DISALLOW_COPY_AND_ASSIGN(State);
};

#endif  // TRANSITION_SYSTEM_STATE_H_
