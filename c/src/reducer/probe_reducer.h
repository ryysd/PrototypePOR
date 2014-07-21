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

  void Reduce(const State* init_state) const {
    std::unordered_map<std::string, bool> explored_vectors;
    std::stack<Vector*> stack;
    std::unique_ptr<Word> empty_word = std::unique_ptr<Word>(new Word());
    std::unordered_map<std::string, bool> visited_states;

    ProbeSet probe_sets;

    Vector* vector = NULL;
    stack.push(new Vector(init_state, empty_word.get()));
    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();

      if (explored_vectors.find(vector->hash()) != explored_vectors.end()) continue;
      explored_vectors.insert(std::make_pair(vector->hash(), true));

      if (visited_states.find(vector->After()->hash()) == visited_states.end()) {
        std::cout << "visit: " << vector->After()->hash() << std::endl;
        visited_states.insert(std::make_pair(vector->After()->hash(), true));
      }
      std::cout << vector->hash() << std::endl;

      CalcProbeSet(vector, &probe_sets);
      std::cout << "probe set ============================" << std::endl;
      for (auto& kv : probe_sets) {
        std::cout << kv.first->name() << ":" << kv.second->name()  << std::endl;
        stack.push(MakeNewVectorFromProbeSet(vector, kv));
      }
      std::cout << "======================================" << std::endl;
      std::cout << std::endl;

      delete vector;
    }

    std::cout << visited_states.size() << std::endl;
  }

 private:
  Vector* MakeNewVectorFromProbeSet(const Vector* vector, const ProbeSet::value_type& probe_set) const {
    return new Vector(
        vector->state()->After(probe_set.second.get()),
        vector->word()->Append(probe_set.first)->Diff(*probe_set.second));
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
      probe_sets->insert(std::make_pair(p, p->CalcPrimeCause(*vector->word())));
    }
  }

  ActionTable* action_table_;

  DISALLOW_COPY_AND_ASSIGN(ProbeReducer);
};

#endif  // REDUCER_PROBE_REDUCER_H_
