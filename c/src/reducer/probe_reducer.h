#ifndef REDUCER_PROBE_REDUCER_H_
#define REDUCER_PROBE_REDUCER_H_

#include <string>
#include <stack>

// class StateInterface {
//  public:
//   State::Hash& hash();
// };
// 
// class ReducerCallbackObject {
//  public:
//   virtual State::Hash& GetInitStateHash();
//   virtual StateInterface* GetSuccessorHashes(std::vector<State::Hash&>);
//  private:
// };

class ProbeReducer {
 public:
  ProbeReducer() {}

  void Reduce() {
    std::unordered_map<std::string, Vector*> explored_vectors;
    std::stack<Vector*> stack;

    Vector* vector = NULL;
    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();
    }
  }

 private:
};

#endif  // REDUCER_PROBE_REDUCER_H_
