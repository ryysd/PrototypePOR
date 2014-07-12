/** 
 *  @file     ats_file_reader.hpp  
 *  @brief    File containing reader of ATS file
 */

#ifndef UTIL_ATS_FILE_READER_H_
#define UTIL_ATS_FILE_READER_H_

#include <string>
#include <fstream>
#include <regex> // NOLINT
#include <vector>
#include <algorithm>
#include "../../thirdparty/picojson.h"
#include "../../thirdparty//logger.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

typedef std::vector<std::string> EntitySet;

/*! @class Action
 *  @brief class representing action
*/
class Action {
 public:
  typedef std::vector<std::string> EntitySet;
  Action(const std::string name, EntitySet creator, EntitySet eraser, EntitySet reader, EntitySet embargoes)
    : name_(name), creator_(creator), reader_(reader), eraser_(eraser), embargoes_(embargoes) {}

  const std::string& name() const { return name_; }
  const EntitySet& creator() const { return creator_; }
  const EntitySet& reader() const { return reader_; }
  const EntitySet& eraser() const { return eraser_; }
  const EntitySet& embargoes() const { return embargoes_; }

 private:
  const std::string name_;
  const EntitySet creator_;
  const EntitySet reader_;
  const EntitySet eraser_;
  const EntitySet embargoes_;

  DISALLOW_COPY_AND_ASSIGN(Action);
};

class State;
/*! @class Transition
 *  @brief class representing transition
*/
class Transition {
 public:
  Transition(const State* source, const State* target, const Action* action)
    : source_(source), target_(target), action_(action) {}

  const State* source() const { return source_; }
  const State* target() const { return target_; }
  const Action* action() const { return action_; }

 private:
  const State* source_;
  const State* target_;
  const Action* action_;

  DISALLOW_COPY_AND_ASSIGN(Transition);
};

/*! @class State
 *  @brief class representing state
*/
class State {
 public:
  State(const std::string name, const std::vector<std::string> entities)
    : name_(name), entities_(entities), reduced_(false) {}

  ~State() { for (Transition* t : transitions_) { delete t; } }

  void AddTransition(State* target, Action* action) { transitions_.push_back(new Transition(this, target, action)); }

  bool reduced() { return reduced_; }
  const std::string& name() const { return name_; }
  const std::vector<Transition*>& transitions() const { return transitions_; }
  const std::vector<std::string>& entities() const { return entities_; }

  bool operator==(const State& other) const { return name_ == other.name(); }

 private:
  bool reduced_;
  const std::string name_;
  const std::vector<std::string> entities_;
  std::vector<Transition*> transitions_;

  DISALLOW_COPY_AND_ASSIGN(State);
};

class StateSpace {
 public:
  ~StateSpace() {
    FreeStates();
    FreeTransitions();
  }

  void Register(State* state) { states_.push_back(state); }
  bool isRegistered(State* state) const { return std::find(states_.begin(), states_.end(), state) != states_.end();  }

  State* Create(const std::string& name, const std::vector<std::string>& entities, bool is_init = false) {
    const State* old = FindByName(name);

    if (old == NULL) {
      State* state = new State(name, entities);
      Register(state);
      if (is_init) init_state_ = state;

      return state;
    }

    return const_cast<State*>(old);
  }

  const State* FindByName(const std::string& name) const {
    auto it = std::find_if(states_.begin(), states_.end(), [&name] (State* s) { return s->name() == name; } );
    return (it != states_.end()) ? reinterpret_cast<const State*>(&(*it)) : NULL;
  }

  const std::vector<State*>& states() const { return states_; }
  const std::vector<Transition*> transitions() const { return transitions_; }
  const State* init_state() const { return init_state_; }

 private:
  void FreeStates() { for (State* s : states_) { delete(s); } }
  void FreeTransitions() { for (Transition* t : transitions_) { delete(t); }}

  std::vector<State*> states_;
  std::vector<Transition*> transitions_;
  State* init_state_;
};

class Word {
};

class Vector {
};

class ActionTable {
 public:
  ~ActionTable() { for (Action* a : actions_) { delete(a); }}

  void Register(Action* action) { actions_.push_back(action); }
  Action* Create(const std::string& name, const EntitySet& creator, const EntitySet& eraser, const EntitySet& reader, const EntitySet& embargoes) {
    const Action* old = FindByName(name);

    if (old == NULL) {
      Action* action = new Action(name, creator, eraser, reader, embargoes);
      Register(action);

      return action;
    }

    return const_cast<Action*>(old);
  }

  const Action* FindByName(const std::string& name) const {
    auto it = std::find_if(actions_.begin(), actions_.end(), [&name] (Action* a) { return a->name() == name; } );
    return (it != actions_.end()) ? reinterpret_cast<const Action*>(&(*it)) : NULL;
  }

 private:
  std::vector<Action*> actions_;
};

class ATSFileReader {
 private:
  static void GetEntity(const std::string& state_name, picojson::object* entities_object, std::vector<std::string>* entities) {
    picojson::array& entities_array = (*entities_object)[state_name].get<picojson::array>();

    std::transform(entities_array.begin(), entities_array.end(), std::back_inserter(*entities),
        [](picojson::value value) { return value.get<std::string>(); });
  }

  static void CreateState(const std::string& state_name, picojson::object* entities_object, StateSpace* state_space, bool is_init = false) {
    std::vector<std::string> entities;
    ATSFileReader::GetEntity(state_name, entities_object, &entities);
    state_space->Create(state_name, entities, is_init);
  }

  static StateSpace* CreateStateSpace(picojson::value* json_value) {
    StateSpace* state_space = new StateSpace();

    try {
      picojson::object& json_object = json_value->get<picojson::object>();
      picojson::object& lts_object = json_object["lts"].get<picojson::object>();
      picojson::object& entities_object = lts_object["states"].get<picojson::object>();

      std::string init_state_name = lts_object["init"].get<std::string>();
      ATSFileReader::CreateState(init_state_name, &entities_object, state_space, true);

      std::regex regex("([^-]+) *- *([^-]+) *-> *([^-]+)");
      std::smatch match_results;

      State* source, *target;
      std::vector<std::string> source_entities, target_entities;
      picojson::array& transitions_array = lts_object["transitions"].get<picojson::array>();
      DEBUG("size: %d\n", transitions_array.size());
      int debug_count = 0;
      for (picojson::value& transition : transitions_array) {
        debug_count++;
        if (debug_count % 1000 == 0) {
          DEBUG("count: %d\n", debug_count);
        }

        if (regex_match(transition.get<std::string>(), match_results, regex)) {
          ATSFileReader::CreateState(match_results.str(1), &entities_object, state_space);
          ATSFileReader::CreateState(match_results.str(3), &entities_object, state_space);
          // TODO(ryysd) add action
        } else {
          ERROR("invalid transition is found.");
          return NULL;
        }
      }
    } catch(...) {
      ERROR("cannot parse 'lts' record.");
      return NULL;
    }

    return state_space;
  }

  static ActionTable* CreateActionTable(picojson::value* json_value) {
    ActionTable* action_table = new ActionTable();

    try {
    } catch(...) {
      ERROR("cannot parse 'actions' record.");
      return NULL;
    }
  }

 public:
  static void* Read(std::string fname) {
    std::ifstream ifs(fname);

    if (ifs.fail()) {
      ERROR("cannot open %s.", fname.c_str());
      return NULL;
    }

    std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    picojson::value json_value;
    std::string err;

    picojson::parse(json_value, json.data(), json.data() + json.length(), &err);

    if (!err.empty()) {
      ERROR(err.c_str());
      return NULL;
    }

    StateSpace* state_space = ATSFileReader::CreateStateSpace(&json_value);

    return NULL;
  }
};

#endif  // UTIL_ATS_FILE_READER_H_
