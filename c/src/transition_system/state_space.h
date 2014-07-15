#ifndef TRANSITION_SYSTEM_STATE_SPACE_H_
#define TRANSITION_SYSTEM_STATE_SPACE_H_

#include <map>
#include <string>
#include <vector>

class StateSpace {
 public:
  StateSpace() {}
  ~StateSpace() {
    FreeStates();
    FreeTransitions();
  }

  State* Register(State* state) { return states_.insert(std::make_pair(state->name(), state)).first->second; }
  bool isRegistered(const std::string& name) const { return states_.find(name) != states_.end();  }

  State* Create(const std::string& name, const std::vector<std::string>& entities) {
    State* old = FindByName(name);
    return old == NULL ? Register(new State(name, entities)) : old;
  }

  State* Create(const std::string& name, const std::vector<std::string>& entities, bool is_init) {
    State* state = Create(name, entities);
    if (is_init) init_state_ = state;
    return state;
  }

  State* FindByName(const std::string& name) const {
    auto it = states_.find(name);
    return (it != states_.end()) ? it->second : NULL;
  }

  const std::map<std::string, State*>& states() const { return states_; }
  const std::vector<Transition*> transitions() const { return transitions_; }
  const State* init_state() const { return init_state_; }

 private:
  void FreeStates() { for (auto& kv : states_) { delete kv.second; } }
  void FreeTransitions() { for (auto t : transitions_) { delete(t); }}

  std::map<std::string, State*> states_;
  std::vector<Transition*> transitions_;
  State* init_state_;

  DISALLOW_COPY_AND_ASSIGN(StateSpace);
};

#endif  // TRANSITION_SYSTEM_STATE_SPACE_H_
