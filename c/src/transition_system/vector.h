#ifndef TRANSITION_SYSTEM_VECTOR_H_
#define TRANSITION_SYSTEM_VECTOR_H_

#include <string>

class Vector {
 public:
  Vector(State* state, Word* word) : state_(state), word_(word), hash_(state->hash() + "@" + word->name()) {}

  const std::string& hash() const { return hash_; }

 private:
  State* state_;
  Word* word_;

  const std::string hash_;
};

#endif  // TRANSITION_SYSTEM_VECTOR_H_
