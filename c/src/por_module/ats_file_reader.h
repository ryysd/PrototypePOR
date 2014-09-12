/** 
 *  @file     ats_file_reader.hpp  
 *  @brief    File containing reader of ATS file
 */

#ifndef POR_MODULE_ATS_FILE_READER_H_
#define POR_MODULE_ATS_FILE_READER_H_

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include <utility>

#include "../../thirdparty/picojson.h"
#include "../../thirdparty//logger.h"

#include "./common.h"
#include "./util/profiler.h"
#include "./transition_system/transition_system.h"


class ATSFileReader {
 public:
  static const std::pair<StateSpace*, ActionTable*> Read(std::string fname) {
    profiler::start_scope();

    std::ifstream ifs(fname,  std::ios::in | std::ios::binary);

    if (ifs.fail()) {
      ERROR("cannot open %s.", fname.c_str());
      return std::make_pair(reinterpret_cast<StateSpace*>(NULL), reinterpret_cast<ActionTable*>(NULL));
    }

    picojson::value json_value;
    std::string error;

    profiler::start("parse");
    ifs >> json_value;
    error = picojson::get_last_error();
    profiler::stop("parse");

    if (!error.empty()) {
      ERROR(error.c_str());
      return std::make_pair(reinterpret_cast<StateSpace*>(NULL), reinterpret_cast<ActionTable*>(NULL));
    }

    profiler::start("gen_actions");
    ActionTable* action_table = ATSFileReader::CreateActionTable(json_value);
    profiler::stop("gen_actions");

    profiler::start("gen_states");
    StateSpace* state_space = ATSFileReader::CreateStateSpace(json_value, action_table);
    profiler::stop("gen_states");

    profiler::end_scope();
    // profiler::dump();

    return std::make_pair(state_space, action_table);
  }

 private:
  static Action* CreateAction(const std::string& action_name, const picojson::object& entities_object, ActionTable* action_table) {
    const picojson::object& action_entities_object = entities_object.at(action_name).get<picojson::object>();
    Action* old = action_table->FindByName(action_name);

    if (old == NULL) {
      std::vector<std::string> creator, eraser, reader, embargoes;

      PicojsonArrayToStringVector(action_entities_object.at("c").get<picojson::array>(), &creator);
      PicojsonArrayToStringVector(action_entities_object.at("d").get<picojson::array>(), &eraser);
      PicojsonArrayToStringVector(action_entities_object.at("r").get<picojson::array>(), &reader);
      PicojsonArrayToStringVector(action_entities_object.at("n").get<picojson::array>(), &embargoes);

      return action_table->Create(action_name, creator, eraser, reader, embargoes);
    }

    return old;
  }

  static StateSpace* CreateStateSpace(const picojson::value& json_value, const ActionTable* action_table) {
    const picojson::object& json_object = json_value.get<picojson::object>();
    const picojson::object& lts_object = json_object.at("lts").get<picojson::object>();
    // const picojson::object& entities_object = lts_object.at("states").get<picojson::object>();
    const picojson::array& init_entities_array = lts_object.at("init_entities").get<picojson::array>();

    std::string init_state_name = lts_object.at("init").get<std::string>();
    std::vector<std::string> init_entities;
    PicojsonArrayToStringVector(init_entities_array, &init_entities);

    return new StateSpace(init_entities, action_table);
  }

  static ActionTable* CreateActionTable(const picojson::value& json_value) {
    ActionTable* action_table = new ActionTable();

    const picojson::object& json_object = json_value.get<picojson::object>();
    const picojson::object& action_object = json_object.at("actions").get<picojson::object>();
    const picojson::object& entities_object = action_object.at("entities").get<picojson::object>();

    for (const auto& kv : entities_object) {
      ATSFileReader::CreateAction(kv.first, entities_object, action_table);
    }

    return action_table;
  }

  static void PicojsonArrayToStringVector(const picojson::array& array, std::vector<std::string>* result) {
    std::transform(array.begin(), array.end(), std::back_inserter(*result),
        [](const picojson::value& value) { return value.get<std::string>(); });
    std::sort(result->begin(), result->end());
  }

  static std::vector<std::string> Split(const std::string& str, char delim) {
    std::vector<std::string> res;
    size_t current = 0, found;
    while ((found = str.find_first_of(delim, current)) != std::string::npos) {
      res.push_back(std::string(str, current, found - current));
      current = found + 1;
    }

    res.push_back(std::string(str, current, str.size() - current));
    return res;
  }
};

#endif  // POR_MODULE_ATS_FILE_READER_H_
