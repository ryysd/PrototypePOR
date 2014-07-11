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
class State {
 public:
    State(std::string name, std::vector<Transition> transitions, std::vector<std::string> entities)
      : name_(name), transitions_(transitions), entities_(entities), reduced_(false) {}

    bool reduced() { return reduced_; }
    std::string& name() { return name_; }
    std::vector<Transition>& transitions() { return transitions_; }
    std::vector<std::string> entities() { return entities_; }

 private:
    bool reduced_;
    std::string name_;
    std::vector<Transition> transitions_;
    std::vector<std::string> entities_;
};

class Action {
 public:
  typedef std::vector<std::string> EntitySet;
  Action(std::string name, EntitySet creator, EntitySet reader, EntitySet eraser, EntitySet embargoes)
    : name_(name), creator_(creator), reader_(reader), eraser_(eraser), embargoes_(embargoes) {}

  std::string& name() { return name_; }
  EntitySet& creator() { return creator_; }
  EntitySet& reader() { return reader_; }
  EntitySet& eraser() { return eraser_; }
  EntitySet& embargoes() { return embargoes_; }
 private:
  std::string name_;
  EntitySet creator_;
  EntitySet reader_;
  EntitySet eraser_;
  EntitySet embargoes_;
};

class Transition {
 public:
  Transition(State source, State target, Action action)
    : source_(source), target_(target), action_(action) {}

  State& source() { return source_; }
  State& target() { return target_; }
  Action& action() { return action_; }

 private:
  State source_;
  State target_;
  Action action_;
};

class StateSpace {
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

    if (ifs.fail()) {
      ERROR("cannot open %s", fname.c_str());
      return NULL;
    }

    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return NULL;
  }
};

#endif  // UTIL_ATS_FILE_READER_H_
