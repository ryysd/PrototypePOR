#ifndef POR_MODULE_REDUCER_PROBE_REDUCER_H_
#define POR_MODULE_REDUCER_PROBE_REDUCER_H_

#include <vector>
#include <string>
#include <stack>
#include <memory>
#include <map>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include "../../util/combination.h"
#include "../../util/simple_timer.h"
#include "../../util/profiler.h"

// debug option
#define BEST_REDUCTION

// this option improves reduction performance
// sometimes segmentation fault occurs (in this case, reversing action is not weak prefix of prime cause).
// if segmentation fault occurs, disable this option
#define USE_REVERSING_ACTION

// enable assertion
#define ENABLE_ASSERT

// use age function
#define USE_AGE_FUNCTION

// debug options (do not use)
// #define DISCHARGE_ALL_TRACE
// #define USE_THESIS_METHOD
// #define SKIP_MISSED_ACTION_PHASE
// #define SKIP_210B

typedef std::vector<std::pair<std::unique_ptr<Word>, const Action*>> MissedAction;
typedef std::vector<const Action*> Actions;

class ProbeReducer {
 public:
  typedef std::map<const Action*, std::unique_ptr<Word>> ProbeSet;

  ProbeReducer() : action_table_(new ActionTable) {}
  explicit ProbeReducer(ActionTable* action_table) : action_table_(action_table) { action_table_->GetActionsVector(&action_vectors_); }

  ~ProbeReducer() { /* delete action_table_; */ }

  void Reduce(const State* init_state, std::unordered_map<std::string, const State*>* visited_states, std::unordered_map<std::string, const Action*>* executed_actions = NULL) const {
    PrintAssert();

    std::unordered_map<std::string, bool> explored_vectors;
    std::stack<Vector*> stack;
    std::unique_ptr<Word> empty_word = std::unique_ptr<Word>(new Word());
    visited_states->clear();

    ProbeSet probe_sets;

    MissedAction missed_actions;
    Vector* vector = NULL;
    stack.push(new Vector(init_state, empty_word.get()));

    profiler::reset();
    profiler::start_scope();
    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();

      if (visited_states->find(vector->After()->hash()) != visited_states->end()) continue;
      explored_vectors.insert(std::make_pair(vector->hash(), true));

      profiler::start("executed_actions");
      // for debug
      if (executed_actions) {
        for (const Action* action : vector->word()->actions()) {
          if (executed_actions->find(action->name()) == executed_actions->end()) {
            executed_actions->insert(std::make_pair(action->name(), action));
          }
        }
      }
      profiler::stop("executed_actions");

#ifndef SKIP_MISSED_ACTION_PHASE
      profiler::start("missed_action");
      CalcFreshMissedAction(vector, &missed_actions);
      for (auto& missed_action : missed_actions) {
        // avoid duplicate search
        if (executed_actions->find(missed_action.second->name()) == executed_actions->end()) {
          executed_actions->insert(std::make_pair(missed_action.second->name(), missed_action.second));
        }
        // std::cout << "missed : " << missed_action.first->name() << " : " << missed_action.second->name() << std::endl;
        stack.push(new Vector(vector->state()->After(missed_action.first.get())->After(missed_action.second), empty_word.get()));
      }
      profiler::stop("missed_action");
#endif

      profiler::start("visited_states");
      if (visited_states->find(vector->After()->hash()) == visited_states->end()) {
        // std::cout << "visit: " << vector->After()->hash() << std::endl;
        const State* after = vector->After();
        visited_states->insert(std::make_pair(after->hash(), after));
      }
      profiler::stop("visited_states");

      profiler::start("enable_actions");
      Actions enable_actions;
      vector->After()->CalcEnableActions(action_vectors_, &enable_actions);
      profiler::stop("enable_actions");

      profiler::start("probe_set");
      CalcProbeSet(vector, enable_actions, &probe_sets);
      profiler::stop("probe_set");
      // CalcIndependentProbeSetWithCycleProviso(vector, &probe_sets);
      // std::cout << "probe set ============================" << std::endl;

      profiler::start("new_vector");
      for (auto& kv : probe_sets) {
        // std::cout << kv.first->name() << ":" << kv.second->name()  << std::endl;
        stack.push(MakeNewVectorFromProbeSet(vector, kv, probe_sets, enable_actions));
      }
      profiler::stop("new_vector");
      // std::cout << "======================================" << std::endl;
      // std::cout << std::endl;

      delete vector;
    }
    profiler::end_scope();
    profiler::dump();
  }

  // construct state space only. no reduce.
  void DebugReduce(const State* init_state, std::unordered_map<std::string, const State*>* visited_states) const {
    std::stack<Vector*> stack;
    visited_states->clear();

    ProbeSet probe_sets;
    Vector* vector = NULL;
    stack.push(new Vector(init_state, new Word()));

    Actions actions;
    action_table_->GetActionsVector(&actions);

    simpletimer_start();
    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();

      const State* after = vector->After();
      if (visited_states->find(after->hash()) != visited_states->end()) continue;
      visited_states->insert(std::make_pair(after->hash(), after));

      Actions enable_actions;
      after->CalcEnableActions(actions, &enable_actions);

      for (const auto p : enable_actions) {
        // stack.push(new Vector(vector->state(), vector->word()->Append(p), std::unique_ptr<Age>(new Age())));
        // stack.push(MakeNewVectorFromProbeSet(vector, kv, probe_sets));
        stack.push(new Vector(vector->state()->After(p), std::unique_ptr<Word>(new Word), std::unique_ptr<Age>(new Age())));
      }

      delete vector;
    }
    std::cout << simpletimer_stop() << "ms" << std::endl;
  }

  AgePtr UpdateAge(const Vector* vector, const ProbeSet& probe_sets, Actions enable_actions) const {
    profiler::start("trans");
    Actions probe_set_domain;
    std::transform(probe_sets.begin(), probe_sets.end(), std::back_inserter(probe_set_domain), [](const ProbeSet::value_type& kv) { return kv.first; });
    profiler::stop("trans");

    profiler::start("add_rm");
    auto ret = vector->age()->Add(enable_actions)->Remove(probe_set_domain);
    profiler::stop("add_rm");
    return ret;
  }

  // impractical (for debug)
  std::unique_ptr<Word> CalcMissedActionTrace(const Vector* vector, const Action* missed_action) const {
    const Word* word = vector->word();

    const Actions actions = missed_action->CalcPrimeCause(*vector->word())->actions();

    if (word->size() < 2) return std::unique_ptr<Word>(new Word{});

    Actions actions_combination;

    std::copy(actions.begin(), actions.end(), std::back_inserter(actions_combination));

    for (auto begin = actions_combination.begin(), it = actions_combination.begin() + 1, end = actions_combination.end(); it != end; ++it) {
      do {
        const Actions trace_cand_actions(begin, it);
        std::unique_ptr<Word> trace_cand_word = std::unique_ptr<Word>(new Word(trace_cand_actions));
        if (trace_cand_word->IsWeakPrefixOf(*word)) {
          if (vector->state()->Enables(trace_cand_word->Append(missed_action).get())) {
            return trace_cand_word;
          }
        }
      } while (next_combination(begin, it, end));
    }

    return std::unique_ptr<Word>(new Word{});
  }

  void CalcReversingFreshMissedAction(const Vector* vector, MissedAction* missed_actions) const {
#ifdef ENABLE_ASSERT
    assert(vector->word()->IsReversingFree());
#endif

    missed_actions->clear();
    CalcPotentiallyMissedAction(vector, missed_actions);
    auto is_not_enable = [vector](MissedAction::value_type& missed_action) {
      INFO(missed_action.second->name().c_str());
      const Action* action = missed_action.second;
      return !vector->state()->Enables(missed_action.first->Append(action).get());
    };

    auto new_end = std::remove_if(missed_actions->begin(), missed_actions->end(), is_not_enable);
    missed_actions->erase(new_end, missed_actions->end());
  }

  void CalcFreshMissedAction(const Vector* vector, MissedAction* missed_actions) const {
    missed_actions->clear();
    if (vector->word()->size() <= 0) return;

#ifdef ENABLE_ASSERT
    assert(vector->word()->IsReversingFree());
#endif

    CalcPotentiallyMissedAction(vector, missed_actions);

    const Actions word_actions = Actions(vector->word()->begin(), vector->word()->end() - 1);
    const Word word(word_actions);
    const Vector pre_vector(vector->state(), &word);

    MissedAction pre_missed_actions;
    CalcPotentiallyMissedAction(&pre_vector, &pre_missed_actions);

    auto is_not_fresh = [vector, &pre_missed_actions](const MissedAction::value_type& missed_action) {
      const Action* action = missed_action.second;

      auto comp = [action](const MissedAction::value_type& kv) { return action->Equals(kv.second); };
      return std::find_if(pre_missed_actions.begin(), pre_missed_actions.end(), comp) != pre_missed_actions.end();
    };

    auto is_not_enable = [vector](MissedAction::value_type& missed_action) {
      // INFO(missed_action.second->name().c_str());
      const Action* action = missed_action.second;
      return !vector->state()->Enables(missed_action.first->Append(action).get());
    };

    auto new_end = std::remove_if(missed_actions->begin(), missed_actions->end(), is_not_fresh);
    missed_actions->erase(new_end, missed_actions->end());

    auto convert = [this, vector](MissedAction::value_type& kv) { return std::make_pair(CalcMissedActionTrace(vector, kv.second), kv.second); };
    std::transform(missed_actions->begin(), missed_actions->end(), missed_actions->begin(), convert);

    new_end = std::remove_if(missed_actions->begin(), missed_actions->end(), is_not_enable);
    missed_actions->erase(new_end, missed_actions->end());
  }

  void CalcPotentiallyMissedAction(const Vector* vector, MissedAction* missed_actions) const {
    profiler::start("potentially");
    missed_actions->clear();
    if (vector->word()->size() < 1) return;

    const Word word(Actions(vector->word()->begin(), vector->word()->end() - 1));
    const Action* last_action = vector->word()->actions().back();

    std::unique_ptr<State> weak_after = NULL;
    for (const Action* action : action_vectors_) {
      if (!last_action->Simulates(action)) continue;
      if (!std::any_of(word.begin(), word.end(), [action](const Action* c) { return c->Disables(action); })) continue; // NOLINT

#ifdef USE_THESIS_METHOD
      if (!weak_after) weak_after = vector->state()->WeakAfter(word);
#else
      if (!weak_after) weak_after = vector->state()->WeakAfter(vector->word());  // correct?
#endif
      if (!weak_after->WeakEnables(action)) continue;

      // TODO(ryysd) check prime_cause(vector->word()) + action is enable at vector->state()
      missed_actions->push_back(std::make_pair(std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()/*correct?*/)), action));
      // missed_actions->push_back(std::make_pair(CalcMissedActionTrace(vector, action), action));  // for debug
    }
    profiler::stop("potentially");
  }

 private:
  Vector* MakeNewVectorFromProbeSet(const Vector* vector, const ProbeSet::value_type& probe_set, const ProbeSet& probe_sets, const Actions& enable_actions) const {
#ifdef USE_AGE_FUNCTION
    return new Vector(
        vector->state()->After(probe_set.second.get()),
        vector->word()->Append(probe_set.first)->CalcWeakDifference(*probe_set.second),
        UpdateAge(vector, probe_sets, enable_actions));
#else
    return new Vector(
        vector->state()->After(probe_set.second.get()),
        vector->word()->Append(probe_set.first)->CalcWeakDifference(*probe_set.second),
        std::unique_ptr<Age>(new Age()));
#endif
  }

  void CalcProbeSetConditionAB(const Vector* vector, const Actions& enable_actions, const Action* a, std::unordered_map<const Action*, WordPtr>* prime_table, Actions* probe_set) const {
    CalcProbeSetConditionA(vector, enable_actions, a, prime_table, probe_set);
    CalcProbeSetConditionB(vector, enable_actions, a, prime_table, probe_set);
  }

  void CalcProbeSetConditionA(const Vector* vector, const Actions& enable_actions, const Action* a, std::unordered_map<const Action*, WordPtr>* prime_table, Actions* probe_set) const {
    for (const Action* b : enable_actions) {
      if (a == b || std::find(probe_set->begin(), probe_set->end(), b) != probe_set->end()) continue;
      if ((b->Disables(a) || a->Disables(b))) {
        probe_set->push_back(b);
        CalcProbeSetConditionAB(vector, enable_actions, b, prime_table, probe_set);
      }
    }
  }

  void CalcProbeSetConditionB(const Vector* vector, const Actions& enable_actions, const Action* a, std::unordered_map<const Action*, WordPtr>* prime_table, Actions* probe_set) const {
#ifdef USE_REVERSING_ACTION
    auto calc_trace =  [vector](const Action* a) { return a->CalcReversingActions(*vector->word()); };
#else
    auto calc_trace = [vector, prime_table](const Action* a) {
      auto it = prime_table->find(a);
      return it == prime_table->end() ? prime_table->insert(std::make_pair(a, std::move(a->CalcPrimeCause(*vector->word())))).first->second.get() : it->second.get();
    };
#endif

    for (const Action* b : enable_actions) {
      if (a == b || std::find(probe_set->begin(), probe_set->end(), b) != probe_set->end()) continue;
      auto it = prime_table->find(b);
      const Word* prime_cause = it == prime_table->end() ? prime_table->insert(std::make_pair(b, std::move(b->CalcPrimeCause(*vector->word())))).first->second.get() : it->second.get();
      if (!calc_trace(a)->IsWeakPrefixOf(*prime_cause)) {
        probe_set->push_back(b);
        CalcProbeSetConditionAB(vector, enable_actions, b, prime_table, probe_set);
      }
    }
  }

  void CalcProbeSet(const Vector* vector, const Actions& enable_actions, ProbeSet* probe_sets)  const {
    probe_sets->clear();

    if (enable_actions.empty()) return;

    Actions max;
    vector->age()->Max(&max);

    const Action* independent_action = CalcIndependentAction(enable_actions);
#ifdef USE_AGE_FUNCTION
    const Action* first_probe_set = !max.empty() ? max.front() : independent_action ? independent_action : enable_actions.front();
#else
    const Action* first_probe_set = independent_action ? independent_action : enable_actions.front();
#endif

    Actions probe_set;
    probe_set.push_back(first_probe_set);

    std::unordered_map<const Action*, WordPtr> prime_table;
    CalcProbeSetConditionAB(vector, enable_actions, first_probe_set, &prime_table, &probe_set);

    // 2.10c
    for (const Action* p : probe_set) {
#ifdef USE_REVERSING_ACTION
      auto it = prime_table.find(p);
      probe_sets->insert(std::make_pair(p, it == prime_table.end() ? p->CalcPrimeCause(*vector->word()) : std::move(it->second)));
#else
      probe_sets->insert(std::make_pair(p, p->CalcReversingActions(*vector->word())));
#endif
    }

    // std::cout << probe_sets->size() << "/" << enable_actions.size() << std::endl;

#ifdef ENABLE_ASSERT
    assert(IsValidProbeSet(vector, *probe_sets, enable_actions));
#endif
  }

  // for debug
  bool IsValidProbeSet(const Vector* vector, const ProbeSet& probe_sets, const Actions& enable_actions) const {
    auto check = [vector, &probe_sets, &enable_actions](const ProbeSet::value_type& probe_set) {
      auto check_inner = [vector, &probe_sets, &enable_actions, &probe_set](const Action* b) {
        const Action* a = probe_set.first;
        const Word* trace = probe_set.second.get();

        bool cond_a = !((a->Disables(b) || b->Disables(a)) && probe_sets.find(b) == probe_sets.end());
        bool cond_b = !(!(trace->IsWeakPrefixOf(*(b->CalcPrimeCause(*vector->word())))) && probe_sets.find(b) == probe_sets.end());
        bool cond_c = trace->IsWeakPrefixOf(*(a->CalcPrimeCause(*(vector->word())).get()));
        bool cond_d = !(!enable_actions.empty() && probe_sets.empty());

        return cond_a && cond_b && cond_c && cond_d;
      };

      return std::all_of(enable_actions.begin(), enable_actions.end(), check_inner);
    };

    return std::all_of(probe_sets.begin(), probe_sets.end(), check);
  }

  void CalcIndependentProbeSet(const Vector* vector, ProbeSet* probe_sets) const {
    probe_sets->clear();

    Actions actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    if (enable_actions.empty()) return;

    const Action* independent_action = CalcIndependentAction(enable_actions);

    if (independent_action) {
      probe_sets->insert(std::make_pair(independent_action, std::unique_ptr<Word>(new Word())));
    } else {
      for (const Action* action : enable_actions) {
        probe_sets->insert(std::make_pair(action, std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()))));
      }
    }

    // std::cout << probe_sets->size() << "/" << enable_actions.size() << std::endl;
    assert(IsValidProbeSet(vector, *probe_sets, enable_actions));
  }

  void NoDuplicateCycleProviso(const Vector* vector, ProbeSet* probe_sets) const {
    bool contains_duplicate = false;
    for (auto& probe_set : *probe_sets) {
      const Action* action = probe_set.first;

      if (std::find(vector->word()->begin(), vector->word()->end(), action) != vector->word()->end()) {
        contains_duplicate = true;
        break;
      }
    }

    if (!contains_duplicate) return;

    Actions actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);

    probe_sets->clear();
    for (const Action* action : enable_actions) {
      std::unique_ptr<Word> word_ptr = std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()));
      if (std::find(word_ptr->actions().begin(), word_ptr->actions().end(), action) == word_ptr->actions().end()) {
        probe_sets->insert(std::make_pair(action, word_ptr->Append(action)));
      } else {
        probe_sets->insert(std::make_pair(action, std::move(word_ptr)));
      }
    }
  }

  void CalcIndependentProbeSetWithCycleProviso(const Vector* vector, ProbeSet* probe_sets) const {
    CalcIndependentProbeSet(vector, probe_sets);
    NoDuplicateCycleProviso(vector, probe_sets);

    Actions actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    assert(IsValidProbeSet(vector, *probe_sets, enable_actions));
  }


  const Action* CalcIndependentAction(const Actions& enable_actions) const {
    for (const Action* a : enable_actions) {
      if (std::all_of(enable_actions.begin(), enable_actions.end(), [a](const Action* b) { return a->Equals(b) || (!a->Disables(b) && !b->Disables(a)); })) {
        return a;
      }
    }

    return NULL;
  }

  void PrintAssert() const {
#ifdef USE_REVERSING_ACTION
    printf("\x1B[33mUSE_REVERSING_ACTION is enabled.\n");
    printf("if segmentation fault occurs, disable USE_REVERSING_ACTION.\x1B[39m\n");
#endif
  }

  ActionTable* action_table_;
  Actions action_vectors_;

  DISALLOW_COPY_AND_ASSIGN(ProbeReducer);
};

#endif  // POR_MODULE_REDUCER_PROBE_REDUCER_H_
