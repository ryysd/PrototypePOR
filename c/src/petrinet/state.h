#ifndef PETRINET_STATE_H_
#define PETRINET_STATE_H_

#include <map>
#include <string>
#include <vector>

typedef std::vector<std::vector<int>> Matrix;
class State {
 public:
  explicit State(const std::vector<int>& marking) : marking_(marking), marking_ptr_(&(marking_.front())), marking_size(marking_.size()) { MakeHash(); }
  State(const int* marking, int marking_length) : marking_(marking, marking + marking_length), marking_ptr_(&(marking_.front())), marking_size(marking_.size()) { MakeHash(); }

  // TODO(ryysd): optimize
  void CalcSuccessors(const Matrix& incidence_column_vectors, const std::vector<Transition*>& transitions, std::map<Transition*, State*>* successors) const {
    int* new_marking_ptr = new int[marking_.size()];
    bool valid = true;

    for (int i = 0, n = transitions.size(); i < n; ++i) {
      const int* incidence_column_vector_ptr = &(incidence_column_vectors[i].front());

      for (int j = 0, m = marking_.size(); j < m; ++j) {
        int sum = marking_ptr_[j] + incidence_column_vector_ptr[j];
        if ((valid = sum >= 0)) new_marking_ptr[j] = sum; else break;  // NOLINT
      }

      if (valid) successors->insert(std::make_pair(transitions[i], new State(new_marking_ptr, marking_.size())));
    }

    delete new_marking_ptr;
  }

  bool Equals(const State& other) const { return std::memcmp(marking_ptr_, other.marking_ptr(), sizeof(int) * marking_.size()) == 0; }
  const std::vector<int>& marking() const { return marking_; }
  const int* marking_ptr() const { return marking_ptr_; }
  const std::string& hash() const { return hash_; }

 private:
  void MakeHash() {
    hash_ =  "[";
    for (auto i : marking_) hash_ += std::to_string(i) + ",";
    if (!hash_.empty()) hash_.erase( --hash_.end() );
    hash_ += "]";
  }

  const std::vector<int> marking_;
  const int* marking_ptr_;
  const int marking_size;
  std::string hash_;

  DISALLOW_COPY_AND_ASSIGN(State);
};

#endif  // PETRINET_STATE_H_
