#ifndef TRANSITION_SYSTEM_VECTOR_H_
#define TRANSITION_SYSTEM_VECTOR_H_

#include <string>
#include "./age.h"

class Vector {
 public:
  Vector(const State* state, const Word* word) : state_(state), word_(word), hash_(state->hash() + "@" + word->name()) {}
  Vector(const State* state, WordPtr word_ptr) : state_(state), word_(word_ptr.get()), hash_(state->hash() + "@" + word_ptr->name()) { word_ptr_ = std::move(word_ptr); }

  const State* After() const { return state_->After(word_); }

  const State* state() const { return state_; }
  const Word* word() const { return word_; }
  const std::string& hash() const { return hash_; }

 private:
  const State* state_;
  const Word* word_;
  WordPtr word_ptr_;

  const std::string hash_;
};

typedef std::unique_ptr<Vector> VectorPtr;

#endif  // TRANSITION_SYSTEM_VECTOR_H_
