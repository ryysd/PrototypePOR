#ifndef POR_MODULE_TRANSITION_SYSTEM_VECTOR_H_
#define POR_MODULE_TRANSITION_SYSTEM_VECTOR_H_

#include <string>
#include "./age.h"

class Vector {
 public:
  Vector(const State* state, const Word* word, AgePtr age_ptr = AgePtr(new Age()))
    : state_(state), word_(word), age_(age_ptr.get()), hash_(state->hash() + "@" + word->name()) { age_ptr_ = std::move(age_ptr); }

  Vector(const State* state, WordPtr word_ptr, AgePtr age_ptr = AgePtr(new Age()))
    : state_(state), word_(word_ptr.get()), age_(age_ptr.get()), hash_(state->hash() + "@" + word_ptr->name()) { word_ptr_ = std::move(word_ptr); age_ptr_ = std::move(age_ptr); }

  const State* After() const { return state_->After(word_); }

  const State* state() const { return state_; }
  const Word* word() const { return word_; }
  const Age* age() const { return age_; }
  const std::string& hash() const { return hash_; }

 private:
  const State* state_;
  const Word* word_;
  const Age* age_;
  WordPtr word_ptr_;
  AgePtr age_ptr_;

  const std::string hash_;
};

typedef std::unique_ptr<Vector> VectorPtr;

#endif  // POR_MODULE_TRANSITION_SYSTEM_VECTOR_H_
