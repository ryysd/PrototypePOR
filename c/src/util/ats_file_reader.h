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
#include <sstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <queue>
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

class Profiler {
 public:
   Profiler() {}
   ~Profiler() { for (auto kv : timers_) { delete kv.second; } }

  void Start(const std::string& name) {
    if (timers_.find(name) == timers_.end()) timers_.insert(std::make_pair(name, new ProfilerTimer()));
    timers_.at(name)->Start();
  }

  void Stop(const std::string& name) {
    timers_.at(name)->Stop();
  }

  void Dump() {
    unsigned column_width = 15;
    unsigned fix_width = 2;
    std::string separator = " | ";
    std::vector<std::string> labels{"name", "incl_ratio [%]", "incl_total [ms]", "incl_ave [ms]", "call"};
    float total_time = timers_.at("total")->total();
    std::stringstream sstream;

    sstream << MakeLabels(labels, column_width, separator) << std::endl;

    auto comp = [](DumpObject a, DumpObject b) { return a.inclusive_time_ratio() > b.inclusive_time_ratio(); };
    std::priority_queue<DumpObject, std::vector<DumpObject>, decltype(comp)> dump_objects(comp);
    float real_total_time = 0;
    for (auto kv : timers_) {
      std::string name = kv.first;
      dump_objects.push(DumpObject(kv.first, kv.second->average(), kv.second->total(), ((kv.second->total() * 100) / total_time), kv.second->call_count()));
      real_total_time += kv.second->total();
    }
    float other_total_time = total_time - (real_total_time - total_time);
    dump_objects.push(DumpObject("other", 0, other_total_time, ((other_total_time * 100) / total_time), 0));

    while(!dump_objects.empty()) {
      sstream << dump_objects.top().ToString(column_width, fix_width, separator) << std::endl;
      dump_objects.pop();
    }

    std::cout << sstream.str();
  }

 private:
  std::string MakeLabels(const std::vector<std::string>& labels, unsigned column_width, const std::string& separator) const {
    std::stringstream ss;
    for (auto label : labels) { ss << std::setw(column_width) << label << separator; }

    return ss.str();
  }

  class ProfilerTimer {
   public:
    ProfilerTimer() : total_(0), call_count_(0) {}
    void Start() { start_ = std::chrono::high_resolution_clock::now(); }
    void Stop() { total_ += std::chrono::high_resolution_clock::now() - start_; call_count_++; }

    float total() const { return ((float)std::chrono::duration_cast<std::chrono::microseconds>(total_).count()) / 1000; }
    unsigned call_count() const { return call_count_; }
    float average() const { return total() / call_count_; }
    
   private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::duration total_;
    unsigned call_count_;
  };

  class DumpObject {
   public:
    DumpObject(std::string name, float inclusive_average_time, float inclusive_total_time, float inclusive_time_ratio, unsigned call_count)
      : name_(name), inclusive_averagel_time_(inclusive_average_time), inclusive_total_time_(inclusive_total_time), inclusive_time_ratio_(inclusive_time_ratio), call_count_(call_count) {}

    const std::string& name() const { return name_; }
    float inclusive_averate_time() const { return inclusive_averagel_time_; }
    float inclusive_total_time() const { return inclusive_total_time_; }
    float inclusive_time_ratio() const { return inclusive_time_ratio_; }
    unsigned call_count() const { return call_count_; }

    std::string ToString(unsigned column_width, unsigned fix_width, std::string separator) const {
      std::stringstream ss;

      ss << std::fixed << std::setprecision(fix_width) << 
        std::setw(column_width) << name_ << separator << 
        std::setw(column_width) << inclusive_time_ratio_ << separator << 
        std::setw(column_width) << inclusive_total_time_ << separator << 
        std::setw(column_width) << inclusive_averagel_time_ << separator << 
        std::setw(column_width) << call_count_ << separator;

      return ss.str();
    }

   private:
    std::string name_;
    float inclusive_averagel_time_;
    float inclusive_total_time_;
    float inclusive_time_ratio_;
    unsigned call_count_;
  };

  std::map<std::string, ProfilerTimer*> timers_;

  DISALLOW_COPY_AND_ASSIGN(Profiler);
};

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

  State* Register(State* state) { return states_.insert(make_pair(state->name(), state)).first->second; }
  bool isRegistered(const std::string& name) const { return states_.find(name) != states_.end();  }

  State* Create(const std::string& name, const std::vector<std::string>& entities) {
    State* old = FindByName(name);
    return old == NULL ? Register(new State(name, entities)) : old;
  }

  State* Create(const std::string& name, const std::vector<std::string>& entities, bool is_init) {
    State* state = Create(name, entities);
    if (is_init) init_state_ = state;
    return state;
  }

  State* FindByName(const std::string& name) const {
    auto it = states_.find(name);
    return (it != states_.end()) ? it->second : NULL;
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
  ~ActionTable() { FreeActions(); }

  Action* Register(Action* action) { return actions_.insert(make_pair(action->name(), action)).first->second; }

  Action* Create(const std::string& name, const EntitySet& creator, const EntitySet& eraser, const EntitySet& reader, const EntitySet& embargoes) {
    Action* old = FindByName(name);
    return old == NULL ? Register(new Action(name, creator, eraser, reader, embargoes)) : old;
  }

  Action* FindByName(const std::string& name) const {
    auto it = actions_.find(name);
    return (it != actions_.end()) ? it->second : NULL;
  }

  const std::map<std::string, Action*> actions() const { return actions_; }

 private:
  void FreeActions() { for (auto &kv : actions_) { delete kv.second; } }

  std::map<std::string, Action*> actions_;
};

void picojson_array_to_string_vector(const picojson::array& array, std::vector<std::string>* result) {
  std::transform(array.begin(), array.end(), std::back_inserter(*result),
      [](picojson::value value) { return value.get<std::string>(); });
}

class ATSFileReader {
 private:
  static State* CreateState(const std::string& state_name, const picojson::object& entities_object, StateSpace* state_space, bool is_init = false) {
    State* old = state_space->FindByName(state_name);

    if (old == NULL) {
      std::vector<std::string> entities;
      picojson_array_to_string_vector(entities_object.at(state_name).get<picojson::array>(), &entities);
      return state_space->Create(state_name, entities, is_init);
    }

    return old;
  }

  static Action* CreateAction(const std::string& action_name, const picojson::object& entities_object, ActionTable* action_table) {
    const picojson::object& action_entities_object = entities_object.at(action_name).get<picojson::object>();
    Action* old = action_table->FindByName(action_name);

    if (old == NULL) {
      std::vector<std::string> creator, eraser, reader, embargoes;

      picojson_array_to_string_vector(action_entities_object.at("c").get<picojson::array>(), &creator);
      picojson_array_to_string_vector(action_entities_object.at("d").get<picojson::array>(), &eraser);
      picojson_array_to_string_vector(action_entities_object.at("r").get<picojson::array>(), &reader);
      picojson_array_to_string_vector(action_entities_object.at("n").get<picojson::array>(), &embargoes);

      return action_table->Create(action_name, creator, eraser, reader, embargoes);
    }

    return old;
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
    Profiler profiler;
    profiler.Start("total");

    std::ifstream ifs(fname,  std::ios::in | std::ios::binary);

    if (ifs.fail()) {
      ERROR("cannot open %s.", fname.c_str());
      return std::make_pair<StateSpace*, ActionTable*>(NULL, NULL);
    }

    picojson::value json_value;
    std::string error;

    profiler.Start("parse");
    ifs >> json_value;
    error = picojson::get_last_error();
    profiler.Stop("parse");

    if (!error.empty()) {
      ERROR(error.c_str());
      return std::make_pair<StateSpace*, ActionTable*>(NULL, NULL);
    }

    profiler.Start("gen_actions");
    ActionTable* action_table = ATSFileReader::CreateActionTable(json_value);
    profiler.Stop("gen_actions");

    profiler.Start("gen_states");
    StateSpace* state_space = ATSFileReader::CreateStateSpace(json_value, action_table);
    profiler.Stop("gen_states");

    std::cout << action_table->actions().size() << std::endl;
    std::cout << state_space->states().size() << std::endl;

    profiler.Stop("total");
    profiler.Dump();

    return std::make_pair(state_space, action_table);
  }
};

#endif  // UTIL_ATS_FILE_READER_H_
