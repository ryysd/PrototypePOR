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
  State(const std::string& name, const std::vector<std::string>& entities)
    : reduced_(false), name_(name), entities_(entities) {}

  ~State() { for (auto t : transitions_) { delete t; } }

  void AddTransition(const State* target, const Action* action) { transitions_.push_back(new Transition(this, target, action)); }

  bool reduced() const { return reduced_; }
  const std::string& name() const { return name_; }
  const std::vector<Transition*>& transitions() const { return transitions_; }
  const std::vector<std::string>& entities() const { return entities_; }

  bool operator==(const State& other) const { return name_ == other.name(); }

 private:
  bool reduced_;
  const std::string name_;
  const std::vector<std::string> entities_;
  std::vector<Transition*> transitions_;

  DISALLOW_COPY_AND_ASSIGN(State);
};

#endif  // TRANSITION_SYSTEM_STATE_H_
