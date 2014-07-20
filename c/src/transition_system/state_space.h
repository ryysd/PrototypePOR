#ifndef TRANSITION_SYSTEM_STATE_SPACE_H_
#define TRANSITION_SYSTEM_STATE_SPACE_H_

#include <map>
#include <string>
#include <vector>
#include <stack>

// TODO(ryysd) generate state space by State::Expand
class StateSpace {
 public:
  StateSpace() {}
  StateSpace(const EntitySet& init_entities, const ActionTable* action_table) { Create(init_entities, &init_state_); Generate(action_table); }

  ~StateSpace() {
    FreeStates();
    FreeTransitions();
  }

  State* Register(State* state) { return states_.insert(std::make_pair(state->hash(), state)).first->second; }
  bool isRegistered(const State::Hash& hash) const { return states_.find(hash) != states_.end();  }

  // State* Create(const State::Hash& hash, const std::vector<std::string>& entities) {
  //   State* old = FindByName(hash);
  //   return old == NULL ? Register(new State(hash, entities)) : old;
  // }

  // State* Create(const State::Hash& hash, const std::vector<std::string>& entities, bool is_init) {
  //   State* state = Create(hash, entities);
  //   if (is_init) init_state_ = state;
  //   return state;
  // }

  // unefficient implementation
  // TODO(ryysd) find old states without creating new state object
  bool Create(const EntitySet& entities, State** new_state) {
    State* state = new State(entities);
    State* old = FindByName(state->hash());

    bool is_new = old == NULL;
    if (is_new) {
      Register(state);
      *new_state = state;
    } else {
      delete state;
      *new_state = old;
    }

    return is_new;
  }

  // bool Create(const EntitySet& entities, bool is_init, State** new_state) {
  //   bool is_new = Create(entities, new_state);
  //   if (is_init) init_state_ = *new_state;
  //   return is_new;
  // }

  State* FindByName(const State::Hash& hash) const {
    auto it = states_.find(hash);
    return (it != states_.end()) ? it->second : NULL;
  }

  void Generate(const ActionTable* action_table) {
    bool is_new = false;
    EntitySet entities;
    State* state = NULL, *new_state = NULL;

    std::vector<const Action*> actions, enable_actions;
    action_table->actions_vector(&actions);

    std::stack<State*> stack;
    stack.push(init_state_);
    while (!stack.empty()) {
      state = stack.top();
      stack.pop();

      state->CalcEnableActions(actions, &enable_actions);
      for (const Action* action : enable_actions) {
        state->Expand(action, &entities);
        is_new = Create(entities, &new_state);

        state->AddTransition(new_state, action);
        if (is_new) stack.push(new_state);
      }
    }
  }

  // for debug (only for small state space)
  void dump() const {
    // Use map to retain visited state to not change State::visited flag.
    // So, memory performance is bad. Do not call this method for large state space
    std::map<std::string, bool> visited;
    std::stack<const State*> stack;

    stack.push(init_state_);
    printf("digraph g{\n");
    while (!stack.empty()) {
      const State* s = stack.top();
      stack.pop();

      if (visited.find(s->hash()) != visited.end()) continue;

      visited.insert(std::make_pair(s->hash(), true));

      if (s->transitions().empty()) printf("  \"%s\" [color=red, style=bold];\n", s->hash().c_str());
      // if (s->reduced()) printf("  \"%s\" [style=filled, fillcolor=\"#999999\", fontcolor=white];\n", s->hash().c_str());
      for (Transition* t : s->transitions()) {
        printf("  \"%s\"->\"%s\" [label=\"%s\"];\n", t->source()->hash().c_str(), t->target()->hash().c_str(), t->action()->name().c_str());
        stack.push(t->target());
      }
    }
    printf("}\n");
  }

  const std::map<std::string, State*>& states() const { return states_; }
  const std::vector<Transition*>& transitions() const { return transitions_; }
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
