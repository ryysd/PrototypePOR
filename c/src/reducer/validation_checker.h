#ifndef REDUCER_VALIDATION_CHECKER_H_
#define REDUCER_VALIDATION_CHECKER_H_

#include <vector>
#include <string>
#include <algorithm>
#include <map>

class ValidationChecker {
 public:
  static void GetDeadlockStates(const std::vector<const State*>& states, std::vector<const State*>* deadlock_states) {
    std::copy_if(states.begin(), states.end(), std::back_inserter(*deadlock_states), [](const State* state) { return state->transitions().empty(); });
  }

  static void DumpValidation(const StateSpace* state_space, const std::unordered_map<std::string, const State*>& visited_states) {
    std::vector<const State*> states_vector, visited_states_vector;
    const std::map<std::string, State*>& states = state_space->states();

    auto second = [](std::unordered_map<std::string, const State*>::value_type  pair) { return pair.second; };
    std::transform(visited_states.begin(), visited_states.end(), std::back_inserter(visited_states_vector), second);
    std::transform(states.begin(), states.end(), std::back_inserter(states_vector), second);

    DumpValidation(states_vector, visited_states_vector);
  }

  static void DumpValidation(const std::vector<const State*>& states, const std::vector<const State*>& visited_states) {
    std::vector<const State*> all_deadlocks, reachable_deadlocks;

    ValidationChecker::GetDeadlockStates(states, &all_deadlocks);
    ValidationChecker::GetDeadlockStates(visited_states, &reachable_deadlocks);

    bool visit_all_deadlocks = all_deadlocks.size() == reachable_deadlocks.size();

    std::cout << "Validation Information =================================" << std::endl;
    std::cout << "All Deadlocks Num: " << all_deadlocks.size() << std::endl;
    std::cout << "Reachable Deadlocks Num: " << reachable_deadlocks.size() << std::endl;
    std::cout << "========================================================" << std::endl;

    std::cout << std::endl;

    std::cout << "Validation Check =======================================" << std::endl;
    std::cout << "Visit All Deadlock: " << (visit_all_deadlocks ? "OK" : "FAIL") << std::endl;
    std::cout << "========================================================" << std::endl;
  }
};

#endif  // REDUCER_VALIDATION_CHECKER_H_
