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

// debug options
#define BEST_REDUCTION
// #define USE_THESIS_METHOD
// #define DISCHARGE_ALL_TRACE
// #define SKIP_MISSED_ACTION_PHASE
// #define SKIP_210B

// necassary options
// #define ENABLE_ASSERT
#define USE_AGE_FUNCTION

typedef std::vector<std::pair<std::unique_ptr<Word>, const Action*>> MissedAction;

class ProbeReducer {
 public:
  typedef std::map<const Action*, std::unique_ptr<Word>> ProbeSet;

  ProbeReducer() : action_table_(new ActionTable) {}
  explicit ProbeReducer(ActionTable* action_table) : action_table_(action_table) {}

  ~ProbeReducer() { /* delete action_table_; */ }

  void Reduce(const State* init_state, std::unordered_map<std::string, const State*>* visited_states, std::unordered_map<std::string, const Action*>* executed_actions = NULL) const {
    std::unordered_map<std::string, bool> explored_vectors;
    std::stack<Vector*> stack;
    std::unique_ptr<Word> empty_word = std::unique_ptr<Word>(new Word());
    // std::unordered_map<std::string, bool> visited_states;
    visited_states->clear();

    ProbeSet probe_sets;

    MissedAction missed_actions;
    Vector* vector = NULL;
    stack.push(new Vector(init_state, empty_word.get()));

    while (!stack.empty()) {
      vector = stack.top();
      stack.pop();

#ifdef BEST_REDUCTION
      if (visited_states->find(vector->After()->hash()) != visited_states->end()) continue;  // best
#else
      if (explored_vectors.find(vector->hash()) != explored_vectors.end()) continue;
#endif
      explored_vectors.insert(std::make_pair(vector->hash(), true));

      std::cout << vector->hash() << std::endl;
      // assert(!vector->word()->ContainsDuplicateAction());

      // for debug
      if (executed_actions) {
        for (const Action* action : vector->word()->actions()) {
          if (executed_actions->find(action->name()) == executed_actions->end()) {
            executed_actions->insert(std::make_pair(action->name(), action));
          }
        }
      }

#ifndef SKIP_MISSED_ACTION_PHASE
      CalcFreshMissedAction(vector, &missed_actions);
      for (auto& missed_action : missed_actions) {
        // avoid duplicate search
        if (executed_actions->find(missed_action.second->name()) == executed_actions->end()) {
          executed_actions->insert(std::make_pair(missed_action.second->name(), missed_action.second));
        }
        std::cout << "missed : " << missed_action.first->name() << " : " << missed_action.second->name() << std::endl;
        stack.push(new Vector(vector->state()->After(missed_action.first.get())->After(missed_action.second), empty_word.get()));
      }
#endif

      if (visited_states->find(vector->After()->hash()) == visited_states->end()) {
        std::cout << "visit: " << vector->After()->hash() << std::endl;
        const State* after = vector->After();
        visited_states->insert(std::make_pair(after->hash(), after));
      }

      CalcProbeSet(vector, &probe_sets);
      // CalcIndependentProbeSetWithCycleProviso(vector, &probe_sets);
      std::cout << "probe set ============================" << std::endl;
      for (auto& kv : probe_sets) {
        std::cout << kv.first->name() << ":" << kv.second->name()  << std::endl;
        stack.push(MakeNewVectorFromProbeSet(vector, kv, probe_sets));
      }
      std::cout << "======================================" << std::endl;
      std::cout << std::endl;

      delete vector;
    }
  }

  AgePtr UpdateAge(const Vector* vector, const ProbeSet& probe_sets) const {
    std::vector<const Action*> actions, enable_actions;

    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);

    std::vector<const Action*> probe_set_domain;
    std::transform(probe_sets.begin(), probe_sets.end(), std::back_inserter(probe_set_domain), [](const ProbeSet::value_type& kv) { return kv.first; });

    return vector->age()->Add(enable_actions)->Remove(probe_set_domain);
  }

  // impractical (for debug)
  std::unique_ptr<Word> CalcMissedActionTrace(const Vector* vector, const Action* missed_action) const {
    const Word* word = vector->word();

#ifdef BEST_REDUCTION
    const std::vector<const Action*> actions = missed_action->CalcPrimeCause(*vector->word())->actions();  // best
#else
    const std::vector<const Action*> actions = word->actions();
#endif

    if (word->size() < 2) return std::unique_ptr<Word>(new Word{});

    std::vector<const Action*> actions_combination;

    std::copy(actions.begin(), actions.end(), std::back_inserter(actions_combination));

    for (auto begin = actions_combination.begin(), it = actions_combination.begin() + 1, end = actions_combination.end(); it != end; ++it) {
      do {
        const std::vector<const Action*> trace_cand_actions(begin, it);
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

    const std::vector<const Action*> word_actions = std::vector<const Action*>(vector->word()->begin(), vector->word()->end() - 1);
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
      INFO(missed_action.second->name().c_str());
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
    missed_actions->clear();
    if (vector->word()->size() < 1) return;

    const std::vector<const Action*> word_actions = std::vector<const Action*>(vector->word()->begin(), vector->word()->end() - 1);
    const Word word(word_actions);
    const Action* last_action = vector->word()->actions().back();

    std::vector<const Action*> actions;
    action_table_->GetActionsVector(&actions);

    for (const Action* action : actions) {
#ifdef USE_THESIS_METHOD
      if (!vector->state()->WeakAfter(word)->WeakEnables(action)) continue;
#else
      if (!vector->state()->WeakAfter(vector->word())->WeakEnables(action)) continue; /*correct?*/
#endif
      if (!std::any_of(word.begin(), word.end(), [action](const Action* c) { return c->Disables(action); })) continue; // NOLINT
      if (!last_action->Simulates(action)) continue;

      // TODO(ryysd) check prime_cause(vector->word()) + action is enable at vector->state()
      missed_actions->push_back(std::make_pair(std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()/*correct?*/)), action));
      // missed_actions->push_back(std::make_pair(CalcMissedActionTrace(vector, action), action));  // for debug
    }
  }

 private:
  Vector* MakeNewVectorFromProbeSet(const Vector* vector, const ProbeSet::value_type& probe_set, const ProbeSet& probe_sets) const {
    return new Vector(
        vector->state()->After(probe_set.second.get()),
        vector->word()->Append(probe_set.first)->CalcWeakDifference(*probe_set.second),
        UpdateAge(vector, probe_sets));
  }

  void CalcProbeSet(const Vector* vector, ProbeSet* probe_sets) const {
    probe_sets->clear();

    std::vector<const Action*> actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    if (enable_actions.empty()) return;

    std::vector<const Action*> max;
    vector->age()->Max(&max);

    const Action* independent_action = CalcIndependentAction(enable_actions);
#ifdef USE_AGE_FUNCTION
    const Action* first_probe_set = !max.empty() ? max.front() : independent_action ? independent_action : enable_actions.front();
#else
    const Action* first_probe_set = independent_action ? independent_action : enable_actions.front();
#endif

    std::vector<const Action*> probe_set;
    probe_set.push_back(first_probe_set);

    // auto calc_trace = [vector](const Action* a) { return a->CalcPrimeCause(*vector->word()); };
    // auto calc_trace = [vector](const Action* a) {
    //   return (vector->word()->size() < 15) ? std::unique_ptr<Word>(new Word()) : a->CalcPrimeCause(*vector->word());
    // };

#ifdef DISCHARGE_ALL_TRACE
    auto calc_trace = [vector](const Action* a) { return std::unique_ptr<Word>(new Word()); };  // best
#else
    auto calc_trace = [vector](const Action* a) { return a->CalcReversingActions(*vector->word()); };
#endif

    // 2.10a
    bool updated = false;
    do {
      updated = false;
      for (const Action* b : enable_actions) {
        if (std::any_of(probe_set.begin(), probe_set.end(), [b](const Action* a) { return b->Disables(a) || a->Disables(b); })) {
          if (std::find(probe_set.begin(), probe_set.end(), b) == probe_set.end()) {
            updated = true;
            probe_set.push_back(b);
          }
        }
      }

#ifndef SKIP_210B
      // 2.10b
      for (const Action* b : enable_actions) {
        if (std::any_of(probe_set.begin(), probe_set.end(), [vector, b, calc_trace](const Action* a) { return !calc_trace(a)->IsWeakPrefixOf(*(b->CalcPrimeCause(*vector->word()))); })) {
          if (std::find(probe_set.begin(), probe_set.end(), b) == probe_set.end()) {
            updated = true;
            probe_set.push_back(b);
          }
        }
      }
#endif
    } while (updated);

    // 2.10c
    for (const Action* p : probe_set) {
      probe_sets->insert(std::make_pair(p, calc_trace(p)));
    }

    std::cout << probe_sets->size() << "/" << enable_actions.size() << std::endl;

#ifdef ENABLE_ASSERT
    assert(IsValidProbeSet(vector, *probe_sets, enable_actions));
#endif
  }

  // for debug
  bool IsValidProbeSet(const Vector* vector, const ProbeSet& probe_sets, const std::vector<const Action*>& enable_actions) const {
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

    std::vector<const Action*> actions, enable_actions;
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

    std::cout << probe_sets->size() << "/" << enable_actions.size() << std::endl;
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

    std::vector<const Action*> actions, enable_actions;
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

    std::vector<const Action*> actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    assert(IsValidProbeSet(vector, *probe_sets, enable_actions));

    // probe_sets->clear();

    // std::vector<const Action*> actions, enable_actions;
    // action_table_->GetActionsVector(&actions);
    // vector->After()->CalcEnableActions(actions, &enable_actions);
    // if (enable_actions.empty()) return;

    // const Action* independent_action = CalcIndependentAction(enable_actions);

    // const std::vector<const Action*>& trace_actions = vector->word()->actions();

    // if (independent_action && std::find(trace_actions.begin(), trace_actions.end(), independent_action) == trace_actions.end()) {
    //   probe_sets->insert(std::make_pair(independent_action, std::unique_ptr<Word>(new Word())));
    // } else {
    //   for (const Action* action : enable_actions) {
    //     std::unique_ptr<Word> word_ptr = std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()));
    //     if (std::find(word_ptr->actions().begin(), word_ptr->actions().end(), action) == word_ptr->actions().end()) {
    //       probe_sets->insert(std::make_pair(action, word_ptr->Append(action)));
    //     } else {
    //       probe_sets->insert(std::make_pair(action, std::move(word_ptr)));
    //     }
    //   }
    // }

    // std::cout << probe_sets->size() << "/" << enable_actions.size() << std::endl;
    // assert(IsValidProbeSet(vector, *probe_sets, enable_actions));
  }


  const Action* CalcIndependentAction(const std::vector<const Action*>& enable_actions) const {
    for (const Action* a : enable_actions) {
      if (std::all_of(enable_actions.begin(), enable_actions.end(), [a](const Action* b) { return a->Equals(b) || (!a->Disables(b) && !b->Disables(a)); })) {
        return a;
      }
    }

    return NULL;
  }

  ActionTable* action_table_;

  DISALLOW_COPY_AND_ASSIGN(ProbeReducer);
};

#endif  // POR_MODULE_REDUCER_PROBE_REDUCER_H_
