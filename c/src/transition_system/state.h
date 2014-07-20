#ifndef TRANSITION_SYSTEM_STATE_H_
#define TRANSITION_SYSTEM_STATE_H_

#include <string>
#include <vector>
#include <cassert>

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
  typedef std::string Hash;  // for debug
  // typedef unsigned& Hash;

  // sorted entities are required.
  State(const Hash& hash, const EntitySet& entities) : /*reduced_(false),*/ hash_(hash), entities_(entities) {}

  explicit State(const EntitySet& entities) : hash_(MakeHash(entities)), entities_(entities) {
    assert(std::is_sorted(entities.begin(), entities.end()));
  }

  ~State() { for (auto t : transitions_) { delete t; } }

  void AddTransition(const State* target, const Action* action) { transitions_.push_back(new Transition(this, target, action)); }

  void Expand(const Action* action, EntitySet* new_entities) const {
    EntitySet tmp;
    std::set_difference(entities_.begin(), entities_.end(), action->eraser().begin(), action->eraser().end(), back_inserter(tmp));
    std::set_union(tmp.begin(), tmp.end(), action->creator().begin(), action->creator().end(), back_inserter(*new_entities));
  }

  bool  Enables(const Action* action) const {
    EntitySet required_entities, insufficient_entities;
    EntitySet unrequired_entities, forbidden_entities;

    std::set_union(action->reader().begin(), action->reader().end(), action->eraser().begin(), action->eraser().end(), back_inserter(required_entities));
    std::set_difference(required_entities.begin(), required_entities.end(), entities_.begin(), entities_.end(), back_inserter(insufficient_entities));
    if (!insufficient_entities.empty()) return false;

    std::set_union(action->embargoes().begin(), action->embargoes().end(), action->creator().begin(), action->creator().end(), back_inserter(unrequired_entities));
    std::set_intersection(unrequired_entities.begin(), unrequired_entities.end(), entities_.begin(), entities_.end(), back_inserter(forbidden_entities));

    return forbidden_entities.empty();
  }

  // void Visit() { visited_ = true; }
  // void Reduce() { reduced_ = true; }

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

  bool Equals(const State& other) const { return hash_ == other.hash(); }

  // bool reduced() const { return reduced_; }
  // bool visited() const { return visited_; }

  const Hash& hash() const { return hash_; }
  const std::vector<Transition*>& transitions() const { return transitions_; }
  const std::vector<std::string>& entities() const { return entities_; }

 private:
  // bool reduced_;
  // bool visited_;

  const std::string MakeHash(const EntitySet& entities) const {
    Hash hash = "";
    for (const std::string& e : entities) { hash += e + ","; }
    if (!hash.empty()) hash.erase(--hash.end());

    return "[" + hash + "]";
  }

  const Hash hash_;
  const std::vector<std::string> entities_;
  std::vector<Transition*> transitions_;

  DISALLOW_COPY_AND_ASSIGN(State);
};

#endif  // TRANSITION_SYSTEM_STATE_H_
