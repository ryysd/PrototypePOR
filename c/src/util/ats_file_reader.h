/** 
 *  @file     ats_file_reader.hpp  
 *  @brief    File containing reader of ATS file
 */

#ifndef UTIL_ATS_FILE_READER_H_
#define UTIL_ATS_FILE_READER_H_

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include "../../thirdparty/picojson.h"
#include "../../thirdparty//logger.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

std::vector<std::string> split(const std::string& str, char delim){
  std::vector<std::string> res;
  size_t current = 0, found;
  while((found = str.find_first_of(delim, current)) != std::string::npos){
    res.push_back(std::string(str, current, found - current));
    current = found + 1;
  }
  res.push_back(std::string(str, current, str.size() - current));
  return res;
}

typedef std::vector<std::string> EntitySet;

/*! @class Action
 *  @brief class representing action
*/
class Action {
 public:
  typedef std::vector<std::string> EntitySet;
  Action(const std::string name, EntitySet creator, EntitySet eraser, EntitySet reader, EntitySet embargoes)
    : name_(name), creator_(creator), reader_(reader), eraser_(eraser), embargoes_(embargoes) {}

  void Simulate(const Action* action) { simulates_.insert(make_pair(action->name(), true)); }
  void Disable(const Action* action) { disables_.insert(make_pair(action->name(), true)); }

  bool isSimulate(const Action* action) const { return simulates_.find(action->name()) != simulates_.end(); }
  bool isDisable(const Action* action) const { return disables_.find(action->name()) != disables_.end(); }

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

  std::map<std::string, bool> simulates_;
  std::map<std::string, bool> disables_;

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

  void AddTransition(const State* target, const Action* action) { transitions_.push_back(new Transition(this, target, action)); }

  bool reduced() const { return reduced_; }
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

  void Register(State* state) { states_.insert(make_pair(state->name(), state)); }
  bool isRegistered(const std::string& name) const { return states_.find(name) != states_.end();  }

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
    auto it = states_.find(name);
    return (it != states_.end()) ? reinterpret_cast<const State*>(it->second) : NULL;
  }

  const std::map<std::string, State*>& states() const { return states_; }
  const std::vector<Transition*> transitions() const { return transitions_; }
  const State* init_state() const { return init_state_; }

 private:
  void FreeStates() { for (auto& kv : states_) { delete kv.second; } }
  void FreeTransitions() { for (Transition* t : transitions_) { delete(t); }}

  std::map<std::string, State*> states_;
  std::vector<Transition*> transitions_;
  State* init_state_;
};

class Word {
};

class Vector {
};

class ActionTable {
 public:
  ~ActionTable() { for (auto &kv : actions_) { delete kv.second; }}

  void Register(Action* action) { actions_.insert(make_pair(action->name(), action)); }
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
    auto it = actions_.find(name);
    return (it != actions_.end()) ? reinterpret_cast<const Action*>(it->second) : NULL;
  }

  const std::map<std::string, Action*> actions() const { return actions_; }

 private:
  std::map<std::string, Action*> actions_;
};

void picojson_array_to_string_vector(const picojson::array& array, std::vector<std::string>* result) {
  std::transform(array.begin(), array.end(), std::back_inserter(*result),
      [](picojson::value value) { return value.get<std::string>(); });
}

class ATSFileReader {
 private:
  static State* CreateState(const std::string& state_name, const picojson::object& entities_object, StateSpace* state_space, bool is_init = false) {
    std::vector<std::string> entities;

    picojson_array_to_string_vector(entities_object.at(state_name).get<picojson::array>(), &entities);
    return state_space->Create(state_name, entities, is_init);
  }

  static Action* CreateAction(const std::string& action_name, const picojson::object& entities_object, ActionTable* action_table) {
    const picojson::object& action_entities_object = entities_object.at(action_name).get<picojson::object>();
    std::vector<std::string> creator, eraser, reader, embargoes;

    picojson_array_to_string_vector(action_entities_object.at("c").get<picojson::array>(), &creator);
    picojson_array_to_string_vector(action_entities_object.at("d").get<picojson::array>(), &eraser);
    picojson_array_to_string_vector(action_entities_object.at("r").get<picojson::array>(), &reader);
    picojson_array_to_string_vector(action_entities_object.at("n").get<picojson::array>(), &embargoes);

    return action_table->Create(action_name, creator, eraser, reader, embargoes);
  }

  static StateSpace* CreateStateSpace(const picojson::value& json_value, ActionTable* action_table) {
    StateSpace* state_space = new StateSpace();

    try {
      const picojson::object& json_object = json_value.get<picojson::object>();
      const picojson::object& lts_object = json_object.at("lts").get<picojson::object>();
      const picojson::object& entities_object = lts_object.at("states").get<picojson::object>();

      std::string init_state_name = lts_object.at("init").get<std::string>();
      ATSFileReader::CreateState(init_state_name, entities_object, state_space, true);

      std::vector<std::string> split_result;

      State* source, *target;
      const picojson::array& transitions_array = lts_object.at("transitions").get<picojson::array>();
      for (const picojson::value& transition : transitions_array) {
        split_result = split(transition.get<std::string>(), '-');

        source = ATSFileReader::CreateState(split_result[0], entities_object, state_space);
        target = ATSFileReader::CreateState(split_result[2], entities_object, state_space);

        source->AddTransition(target, action_table->FindByName(split_result[1]));
      }
    } catch(...) {
      ERROR("cannot parse 'lts' record.");
      return NULL;
    }

    return state_space;
  }

  static ActionTable* CreateActionTable(const picojson::value& json_value) {
    ActionTable* action_table = new ActionTable();

    try {
      const picojson::object& json_object = json_value.get<picojson::object>();
      const picojson::object& action_object = json_object.at("actions").get<picojson::object>();
      const picojson::object& entities_object = action_object.at("entities").get<picojson::object>();

      std::vector<std::string> split_result;

      Action* source, *target;
      const picojson::array& relations_array = action_object.at("relations").get<picojson::array>();
      for (const picojson::value& relation : relations_array) {
        split_result = split(relation.get<std::string>(), '-');
        source = ATSFileReader::CreateAction(split_result[0], entities_object, action_table);
        target = ATSFileReader::CreateAction(split_result[2], entities_object, action_table);

        (split_result[1] == "s") ? source->Simulate(target) : target->Disable(target);
      }
    } catch(...) {
      ERROR("cannot parse 'actions' record.");
      return NULL;
    }

    return action_table;
  }

 public:
  static const std::pair<StateSpace*, ActionTable*> Read(std::string fname) {
    std::ifstream ifs(fname);

    if (ifs.fail()) {
      ERROR("cannot open %s.", fname.c_str());
      return std::make_pair<StateSpace*, ActionTable*>(NULL, NULL);
    }

    std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    picojson::value json_value;
    std::string err;

    picojson::parse(json_value, json.data(), json.data() + json.length(), &err);

    if (!err.empty()) {
      ERROR(err.c_str());
      return std::make_pair<StateSpace*, ActionTable*>(NULL, NULL);
    }

    ActionTable* action_table = ATSFileReader::CreateActionTable(json_value);
    StateSpace* state_space = ATSFileReader::CreateStateSpace(json_value, action_table);

    return std::make_pair(state_space, action_table);
  }
};

#endif  // UTIL_ATS_FILE_READER_H_
