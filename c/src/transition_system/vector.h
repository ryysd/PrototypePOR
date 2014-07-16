#ifndef TRANSITION_SYSTEM_VECTOR_H_
#define TRANSITION_SYSTEM_VECTOR_H_

class Vector {
 public:
  Vector(State* state, Word* word) : state_(state), word_(word) {}

 private:
  State* state_;
  Word* word_;
};

#endif  // TRANSITION_SYSTEM_VECTOR_H_
