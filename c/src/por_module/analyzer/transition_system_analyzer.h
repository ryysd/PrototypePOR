#ifndef POR_MODULE_ANALYZER_TRANSITION_SYSTEM_ANALYZER_H_
#define POR_MODULE_ANALYZER_TRANSITION_SYSTEM_ANALYZER_H_

#include <map>
#include <string>
#include <vector>

class TransitionSystemAnalyzer {
 public:
  TransitionSystemAnalyzer(const ActionTable* action_table, const StateSpace* state_space) : action_table_(action_table), state_space_(state_space) {}

  void Dump() {
    std::map<std::string, float> analyzed_data;

    CalcRelationsAverage(&analyzed_data);

    std::cout << "Analyzed Results =======================================" << std::endl;
    for (auto& kv : analyzed_data) {
      std::cout << kv.first << ": " << kv.second << std::endl;
    }
    std::cout << "========================================================" << std::endl;
  }

 private:
  void CalcRelationsAverage(std::map<std::string, float>* results) {
    std::vector<const Action*> actions;

    int disable_count = 0;
    int simulate_count = 0;
    int influence_count = 0;

    action_table_->GetActionsVector(&actions);

    int out_edge_sum = 0;
    for (auto kv : state_space_->states()) {
      out_edge_sum += kv.second->transitions().size();
    }

    for (const Action* a : actions) {
      for (const Action* b : actions) {
        if (a->Disables(b)) disable_count++;
        if (a->Simulates(b)) simulate_count++;
        if (a->Influences(b)) influence_count++;
      }
    }

    results->insert(std::make_pair("state num", state_space_->states().size()));
    results->insert(std::make_pair("action num", actions.size()));
    results->insert(std::make_pair("average disable count", static_cast<float>(disable_count) / actions.size()));
    results->insert(std::make_pair("average simulate count", static_cast<float>(simulate_count) / actions.size()));
    results->insert(std::make_pair("average influence count", static_cast<float>(influence_count) / actions.size()));
    results->insert(std::make_pair("average output edge", static_cast<float>(out_edge_sum) / state_space_->states().size()));
  }

  const ActionTable* action_table_;
  const StateSpace* state_space_;

  DISALLOW_COPY_AND_ASSIGN(TransitionSystemAnalyzer);
};

#endif  // POR_MODULE_ANALYZER_TRANSITION_SYSTEM_ANALYZER_H_
