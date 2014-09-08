#ifndef REDUCER_PROBE_REDUCER_H_
#define REDUCER_PROBE_REDUCER_H_

#include <vector>
#include <string>
#include <stack>
#include <memory>
#include <map>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include "../util/combination.h"

// class ReducerCallbackObject {
//  public:
//   virtual void OnVisit(const State* state) const = 0;
//   virtual void OnExpand(State* state) const = 0;
// };

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

      if (explored_vectors.find(vector->hash()) != explored_vectors.end()) continue;
      explored_vectors.insert(std::make_pair(vector->hash(), true));

      std::cout << vector->hash() << std::endl;

      // for debug
      if (executed_actions) {
        for (const Action* action : vector->word()->actions()) {
          if (executed_actions->find(action->name()) == executed_actions->end()) {
            executed_actions->insert(std::make_pair(action->name(), action));
          }
        }
      }

      CalcFreshMissedAction(vector, &missed_actions);
      for (auto& missed_action : missed_actions) {
        std::cout << "missed : " << missed_action.first->name() << " : " << missed_action.second->name() << std::endl;

        // avoid duplicate search
        if (executed_actions->find(missed_action.second->name()) == executed_actions->end()) {
          executed_actions->insert(std::make_pair(missed_action.second->name(), missed_action.second));
          stack.push(new Vector(vector->state()->After(missed_action.first.get())->After(missed_action.second), empty_word.get()));
        }
      }

      if (visited_states->find(vector->After()->hash()) == visited_states->end()) {
        std::cout << "visit: " << vector->After()->hash() << std::endl;
        const State* after = vector->After();
        visited_states->insert(std::make_pair(after->hash(), after));
      }

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
  }

  // impractical (for debug)
  std::unique_ptr<Word> CalcMissedActionTrace(const Vector* vector, const Action* missed_action) const {
    const Word* word = vector->word();
    const std::vector<const Action*> actions = word->actions();

    if (word->size() < 2) return std::unique_ptr<Word>(new Word{});

    std::vector<const Action*> actions_combination;

    std::copy(actions.begin(), actions.end(), std::back_inserter(actions_combination));

    for (auto begin = actions_combination.begin(), it = actions_combination.begin() + 1, end = actions_combination.end(); it != end; ++it) {
      do {
        const std::vector<const Action*> trace_cand_actions(begin, it);
        std::unique_ptr<Word> trace_cand_word = std::unique_ptr<Word>(new Word(trace_cand_actions));
        INFO("trace_cand_word");
        std::cout << trace_cand_word->name() << std::endl;
        if (trace_cand_word->IsWeakPrefixOf(*word)) {
          if (vector->state()->Enables(trace_cand_word->Append(missed_action).get())) {
            return trace_cand_word;
          }
        }
      } while (next_combination(begin, it, end));
    }

    return std::unique_ptr<Word>(new Word{});
  }

  void CalcFreshMissedAction(const Vector* vector, MissedAction* missed_actions) const {
    assert(vector->word()->IsReversingFree());

    CalcPotentiallyMissedAction(vector, missed_actions);
    auto is_not_enables = [vector](MissedAction::value_type& missed_action) {
      INFO(missed_action.second->name().c_str());
      const Action* action = missed_action.second;
      return !vector->state()->Enables(missed_action.first->Append(action).get());
    };

    auto new_end = std::remove_if(missed_actions->begin(), missed_actions->end(), is_not_enables);
    missed_actions->erase(new_end, missed_actions->end());
  }

  void CalcPotentiallyMissedAction(const Vector* vector, MissedAction* missed_actions) const {
    if (vector->word()->size() < 1) return;
    missed_actions->clear();

    const std::vector<const Action*> word_actions = std::vector<const Action*>(vector->word()->begin(), vector->word()->end() - 1);
    const Word word(word_actions);
    const Action* last_action = vector->word()->actions().back();

    std::vector<const Action*> actions;
    action_table_->GetActionsVector(&actions);

    for (const Action* action : actions) {
      // if (!vector->state()->WeakAfter(word)->WeakEnables(action)) continue;
      if (!vector->state()->WeakAfter(vector->word())->WeakEnables(action)) continue; /*correct?*/
      if (!std::any_of(word.begin(), word.end(), [action](const Action* c) { return c->Disables(action); })) continue; // NOLINT
      if (!last_action->Simulates(action)) continue;

      // TODO(ryysd) check prime_cause(vector->word()) + action is enable at vector->state()
      // missed_actions->push_back(std::make_pair(std::unique_ptr<Word>(action->CalcPrimeCause(*vector->word()/*correct?*/)), action));
      missed_actions->push_back(std::make_pair(CalcMissedActionTrace(vector, action), action));  // for debug
    }
  }

 private:
  Vector* MakeNewVectorFromProbeSet(const Vector* vector, const ProbeSet::value_type& probe_set) const {
    return new Vector(
        vector->state()->After(probe_set.second.get()),
        vector->word()->Append(probe_set.first)->CalcWeakDifference(*probe_set.second));
  }

  void CalcProbeSet(const Vector* vector, ProbeSet* probe_sets) const {
    probe_sets->clear();

    std::vector<const Action*> actions, enable_actions;
    action_table_->GetActionsVector(&actions);
    vector->After()->CalcEnableActions(actions, &enable_actions);
    if (enable_actions.empty()) return;

    const Action* independent_action = CalcIndependentAction(enable_actions);
    const Action* first_probe_set = independent_action ? independent_action : enable_actions.front();

    std::vector<const Action*> probe_set;
    probe_set.push_back(first_probe_set);

    // 2.10a
    for (const Action* b : enable_actions) {
      if (std::all_of(probe_set.begin(), probe_set.end(), [b](const Action* a) { return b->Disables(a) || a->Disables(b); })) {
        probe_set.push_back(b);
      }
    }

    // 2.10b
    // std::vector<const Action*> new_probe_set;
    // do {
    //   new_probe_set.clear();
    //   for (const Action* b : enable_actions) {
    //     for (const Action* p : probe_set) {
    //       // unefficient implementation
    //       if (std::find(probe_set.begin(), probe_set.end(), b) != probe_set.end()) continue;
    //       if (!p->CalcPrimeCause(*vector->word())->IsWeakPrefixOf(*(b->CalcPrimeCause(*vector->word())))) {
    //         new_probe_set.push_back(b);
    //       }
    //     }
    //   }

    //   probe_set.insert(probe_set.end(), new_probe_set.begin(), new_probe_set.end());
    // } while (!new_probe_set.empty());

    // 2.10c
    for (const Action* p : probe_set) {
      // TODO(ryysd) calc 2.10b, 2.10c
      // probe_sets->insert(std::make_pair(p, std::unique_ptr<Word>(new Word())));
      probe_sets->insert(std::make_pair(p, p->CalcPrimeCause(*vector->word())));
      // probe_sets->insert(std::make_pair(p, p->CalcReversingActions(*(p->CalcPrimeCause(*vector->word())))));
    }
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
  }

  // void CalcReversingProbeSet(const Vector* vector, ProbeSet* probe_sets) const {
  // }

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

#endif  // REDUCER_PROBE_REDUCER_H_
