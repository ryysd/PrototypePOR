#ifndef REDUCER_REDUCER_EXECUTER_H_
#define REDUCER_REDUCER_EXECUTER_H_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "../ats_file_reader.h"
#include "./reducer_option.h"
#include "./probe_reducer.h"

class ReducerExecuter {
 public:
  explicit ReducerExecuter(const ReducerOption* option) : option_(option) {
    auto pair = ATSFileReader::Read(option_->ats_file_name());
    state_space_ = pair.first;
    action_table_ = pair.second;

    reducer_ = new ProbeReducer(action_table_);
  }

  void Execute() {
    visited_states_.clear();
    executed_actions_.clear();

    reducer_->Reduce(state_space_->init_state(), &visited_states_, &executed_actions_);
  }

  void DumpResult() {
    auto get_deadlock_states = [](const std::vector<const State*>& states, std::vector<const State*>* deadlock_states) {
      std::copy_if(states.begin(), states.end(), std::back_inserter(*deadlock_states), [](const State* state) { return state->transitions().empty(); });
    };

    auto ok_fail = [](bool result) { return result ? "\e[0;32mOK\033[0m" : "\e[0;31mFAIL\033[0m"; };

    std::vector<const State*> states, visited_states;
    GetStatesVector(&states);
    GetVisitedStatesVector(&visited_states);

    std::vector<const State*> all_deadlocks, reachable_deadlocks;
    get_deadlock_states(states, &all_deadlocks);
    get_deadlock_states(visited_states, &reachable_deadlocks);

    int full_size = state_space_->states().size();
    int reduced_size = visited_states_.size();
    float reduction_ratio = static_cast<float>(full_size - reduced_size)*100 / full_size;
    std::cout << "Result =================================================" << std::endl;
    std::cout << "Full State Space: " << full_size << std::endl;
    std::cout << "Reduced State Space: " << reduced_size << std::endl;
    std::cout << "Reduction Ratio: " << reduction_ratio << "%" << std::endl;
    std::cout << "========================================================" << std::endl;

    std::cout << std::endl;

    bool visited_all_deadlocks = all_deadlocks.size() == reachable_deadlocks.size();
    bool executed_all_actions = action_table_->actions().size() == executed_actions_.size();
    std::cout << "Validation Check =======================================" << std::endl;
    std::cout << "Visit All Deadlocks: " << ok_fail(visited_all_deadlocks) << " (" << reachable_deadlocks.size() << "/"  << all_deadlocks.size() << ")" << std::endl;
    std::cout << "Execute All Actions: " << ok_fail(executed_all_actions) << " (" << executed_actions_.size() << "/"  << action_table_->actions().size() << ")" << std::endl;
    std::cout << "========================================================" << std::endl;

    DumpDot();
  }

  void DumpDot() {
    if (option_->dot_file_name().empty()) return;

    std::ofstream ofs(option_->dot_file_name());
    if (ofs.fail()) {
      ERROR("cannot open %s.", option_->dot_file_name().c_str());
      return;
    }

    ofs << state_space_->ToDot(visited_states_, true, true);
  }

 private:
  void GetVisitedStatesVector(std::vector<const State*>* states_vector) {
    auto second = [](std::unordered_map<std::string, const State*>::value_type  pair) { return pair.second; };
    std::transform(visited_states_.begin(), visited_states_.end(), std::back_inserter(*states_vector), second);
  }

  void GetStatesVector(std::vector<const State*>* states_vector) {
    auto second = [](std::map<std::string, const State*>::value_type  pair) { return pair.second; };
    std::transform(state_space_->states().begin(), state_space_->states().end(), std::back_inserter(*states_vector), second);
  }

  const ReducerOption* option_;
  StateSpace* state_space_;
  ActionTable* action_table_;
  std::unordered_map<std::string, const State*> visited_states_;
  std::unordered_map<std::string, const Action*> executed_actions_;
  ProbeReducer* reducer_;
  unsigned int execution_time_;

  DISALLOW_COPY_AND_ASSIGN(ReducerExecuter);
};

#endif  // REDUCER_REDUCER_EXECUTER_H_
