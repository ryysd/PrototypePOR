/** 
 *  @file     ats_file_reader.hpp  
 *  @brief    File containing reader of ATS file
 */

#ifndef UTIL_ATS_FILE_READER_H_
#define UTIL_ATS_FILE_READER_H_

#include <string>
#include <fstream>
#include <vector>
#include "../../thirdparty/picojson.h"
#include "../../thirdparty//logger.h"

class Transition;

/*! @class State
 *  @brief class representing state
 *  Do not use STL container to keep performance.
*/
class State {
 public:
  State(const char* name, const Transition* transitions, const char** entities)
    : name_(name), transitions_(transitions), entities_(entities), reduced_(false) {}

  bool reduced() { return reduced_; }
  const char* name() { return name_; }
  const Transition* transitions() { return transitions_; }
  const char** entities() { return entities_; }

 private:
  bool reduced_;
  const char* name_;
  const Transition* transitions_;
  const char** entities_;
};

/*! @class Action
 *  @brief class representing action
 *  Do not use STL container to keep performance.
*/
class Action {
 public:
  typedef std::vector<std::string> EntitySet;
  Action(const char* name, const char** creator, const char** reader, const char** eraser, const char** embargoes)
    : name_(name), creator_(creator), reader_(reader), eraser_(eraser), embargoes_(embargoes) {}

  const char* name() { return name_; }
  const char** creator() { return creator_; }
  const char** reader() { return reader_; }
  const char** eraser() { return eraser_; }
  const char** embargoes() { return embargoes_; }
 private:
  const char* name_;
  const char** creator_;
  const char** reader_;
  const char** eraser_;
  const char** embargoes_;
};

/*! @class Transition
 *  @brief class representing transition
 *  Do not use STL container to keep performance.
*/
class Transition {
 public:
  Transition(const State* source, const State* target, const Action* action)
    : source_(source), target_(target), action_(action) {}

  const State* source() { return source_; }
  const State* target() { return target_; }
  const Action* action() { return action_; }

 private:
  const State* source_;
  const State* target_;
  const Action* action_;
};

class StateSpace {
 public:
  ~StateSpace() {
    FreeStates();
    FreeTransitions();
  }

  void Add(State* state) { states_.push_back(state); }

  std::vector<State*>& states() { return states_; }
  std::vector<Transition*> transitions() { return transitions_; }
  State* init_state() { return init_state_; }

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
};

class ATSFileReader {
 public:
  static void* Read(std::string fname) {
    std::ifstream ifs(fname);
    std::cout << sizeof(State);

    if (ifs.fail()) {
      ERROR("cannot open %s", fname.c_str());
      return NULL;
    }

    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return NULL;
  }
};

#endif  // UTIL_ATS_FILE_READER_H_
