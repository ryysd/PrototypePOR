#ifndef REDUCER_PROBE_REDUCER_H_
#define REDUCER_PROBE_REDUCER_H_

#include <vector>
#include <string>
#include <stack>
#include <memory>
#include <map>
#include <unordered_map>

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
  typedef std::map<const Action*, std::unique_ptr<Word>> ProbeSet;

  ProbeReducer() : action_table_(new ActionTable) {}
  explicit ProbeReducer(ActionTable* action_table) : action_table_(action_table) {}

  ~ProbeReducer() { /* delete action_table_; */ }

  void Reduce() {
    std::unordered_map<std::string, Vector*> explored_vectors;
    std::stack<Vector*> stack;

    ProbeSet probe_sets;

    Vector* vector = NULL;
    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();

      CalcProbeSet(vector, &probe_sets);
    }
  }

  void CalcProbeSet(const Vector* vector, ProbeSet* probe_sets) const {
    probe_sets->clear();

    std::vector<const Action*> actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    if (enable_actions.empty()) return;

    std::vector<const Action*> probe_set;
    probe_set.push_back(enable_actions.front());

    for (const Action* b : enable_actions) {
      if (std::all_of(probe_set.begin(), probe_set.end(), [b](const Action* a) { return b->Disables(a) || a->Disables(b); })) {
        probe_set.push_back(b);
      }
    }

    for (const Action* p : probe_set) {
      // TODO(ryysd) calc 2.10b, 2.10c
      probe_sets->insert(std::make_pair(p, std::unique_ptr<Word>(new Word)));
    }
  }

 private:
  ActionTable* action_table_;

  DISALLOW_COPY_AND_ASSIGN(ProbeReducer);
};

#endif  // REDUCER_PROBE_REDUCER_H_
