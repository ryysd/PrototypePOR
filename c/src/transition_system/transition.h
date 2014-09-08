#ifndef TRANSITION_SYSTEM_TRANSITION_H_
#define TRANSITION_SYSTEM_TRANSITION_H_

class State;
/*! @class Transition
 *  @brief class representing transition
*/
class Transition {
 public:
  Transition(const State* source, const State* target, const Action* action)
    : source_(source), target_(target), action_(action) {}

  const State* source() const { return source_; }
  const State* target() const { return target_; }
  const Action* action() const { return action_; }

 private:
  const State* source_;
  const State* target_;
  const Action* action_;

  DISALLOW_COPY_AND_ASSIGN(Transition);
};

#endif  // TRANSITION_SYSTEM_TRANSITION_H_
