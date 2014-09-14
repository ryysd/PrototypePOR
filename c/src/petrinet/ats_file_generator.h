#ifndef PETRINET_ATS_FILE_GENERATOR_H_
#define PETRINET_ATS_FILE_GENERATOR_H_

#include <map>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "../util/simple_json.h"

class ATSFileGenerator {
 public:
  ATSFileGenerator() {}

  std::string Generate(const std::string& file_name) const {
    PNMLParser parser;
    std::vector<Place*> places;
    std::vector<Transition*> transitions;
    parser.Parse(file_name, &places, &transitions);

    std::vector<std::unique_ptr<State>> states;
    Petrinet petrinet(places, transitions);

    std::vector<std::tuple<const State*, const Transition*, const State*>> relations;
    auto callback = [&relations](const State* source, const Transition* transition, const State* target) { relations.push_back(std::make_tuple(source, transition, target)); };
    State* init_state = petrinet.Execute(&states, callback);

    std::cout << states.size() << std::endl;

    std::string json = GenerateJSON(init_state, relations, places);

    for (auto place : places) delete place;
    for (auto transition : transitions) delete transition;

    return json;
  }

 private:
  typedef std::vector<std::string> Entity;
  typedef struct _action {std::string name; Entity c; Entity r; Entity d; Entity n;} Action;
  std::string GenerateJSON(const State* init_state, const std::vector<std::tuple<const State*, const Transition*, const State*>>& relations, const std::vector<Place*>& places) const {
    std::unordered_map<std::string, std::string> name_map;  // conplex name to simple name (action name + entities -> action + appearance count)
    std::unordered_map<std::string, int> appearance_counts;
    std::vector<std::unique_ptr<Action>> actions;
    Entity init_entities;

    for (auto relation : relations) {
      const State* source = std::get<0>(relation);
      const Transition* transition = std::get<1>(relation);
      const State* target = std::get<2>(relation);

      Entity source_entities, target_entities;
      Entity source_guard_entities, target_guard_entities;
      for (int i = 0, n = source->marking().size(); i < n; ++i) {
        if (source->marking()[i] > 0) {
          source_entities.push_back(places[i]->id() + "_" + std::to_string(source->marking()[i]));
          source_guard_entities.push_back("@" + places[i]->id());
        }
      }

      for (int i = 0, n = target->marking().size(); i < n; ++i) {
        if (target->marking()[i] > 0) {
          target_entities.push_back(places[i]->id() + "_" + std::to_string(target->marking()[i]));
          target_guard_entities.push_back("@" + places[i]->id());
        }
      }

      std::sort(source_entities.begin(), source_entities.end());
      std::sort(target_entities.begin(), target_entities.end());
      std::sort(source_guard_entities.begin(), source_guard_entities.end());
      std::sort(target_guard_entities.begin(), target_guard_entities.end());

      if (source->Equals(*init_state) && init_entities.empty()) {
        init_entities = source_guard_entities;
        init_entities.insert(init_entities.end(), source_entities.begin(), source_entities.end());
      }

      Entity forbitten_creator, forbitten_eraser;
      std::set_difference(target_guard_entities.begin(), target_guard_entities.end(), source_guard_entities.begin(), source_guard_entities.end(), std::back_inserter(forbitten_creator));
      std::set_difference(source_guard_entities.begin(), source_guard_entities.end(), target_guard_entities.begin(), target_guard_entities.end(), std::back_inserter(forbitten_eraser));

      Entity real_creator, real_eraser;
      std::set_difference(target_entities.begin(), target_entities.end(), source_entities.begin(), source_entities.end(), std::back_inserter(real_creator));
      std::set_difference(source_entities.begin(), source_entities.end(), target_entities.begin(), target_entities.end(), std::back_inserter(real_eraser));

      Entity creator(forbitten_creator.begin(), forbitten_creator.end()), eraser(forbitten_eraser.begin(), forbitten_eraser.end());
      creator.insert(creator.end(), real_creator.begin(), real_creator.end());
      eraser.insert(eraser.end(), real_eraser.begin(), real_eraser.end());

      Entity empty_vector;
      std::string entity_string;
      MakeEntityString(creator, eraser, empty_vector, empty_vector, &entity_string);
      std::string action_entities_name = transition->id() + "_" + entity_string;
      if (name_map.find(action_entities_name) == name_map.end()) {
        appearance_counts[transition->id()] += 1;
        std::string action_name = transition->id() + "_" + std::to_string(appearance_counts[transition->id()]);
        actions.push_back(std::unique_ptr<Action>(new Action{action_name, creator, empty_vector, eraser, empty_vector}));
        name_map.insert(std::make_pair(action_entities_name, action_name));
      }
    }

    auto make_json_action = [](const std::unique_ptr<Action>& action) {
      std::map<std::string, std::string> hash;

      hash.insert(std::make_pair("c", simplejson::make_json_array(action->c)));
      hash.insert(std::make_pair("d", simplejson::make_json_array(action->d)));
      hash.insert(std::make_pair("n", simplejson::make_json_array(action->n)));
      hash.insert(std::make_pair("r", simplejson::make_json_array(action->r)));

      return simplejson::make_json_key_values(hash);
    };

    auto make_json_actions = [make_json_action](std::vector<std::unique_ptr<Action>>& actions) {
      std::map<std::string, std::string> hash;
      for (auto& action : actions) {
        hash.insert(std::make_pair(action->name, make_json_action(action)));
      }
      return simplejson::make_json_hash("entities", simplejson::make_json_hash(hash));
    };

    auto make_json_lts = [](const Entity& init_entities) {
      std::map<std::string, std::string> hash;
      hash.insert(std::make_pair("init_entities", simplejson::make_json_array(init_entities)));
      return simplejson::make_json_key_values(hash);
    };

    std::map<std::string, std::string> hash;
    hash.insert(std::make_pair("actions", make_json_actions(actions)));
    hash.insert(std::make_pair("lts", make_json_lts(init_entities)));

    return simplejson::make_json_document(simplejson::make_json_hash(hash));
  }

  void MakeEntityString(const Entity& creator, const Entity& reader, const Entity& eraser, const Entity& embargoes, std::string* entity_string) const {
    auto make_entity_string = [entity_string](const Entity& entities, const std::string& header) {
      if (!entities.empty()) {
        (*entity_string) += header;
        for (auto entity : entities) (*entity_string) += entity + ",";
      }
    };

    make_entity_string(creator, "c_");
    make_entity_string(reader, "r_");
    make_entity_string(eraser, "d_");
    make_entity_string(embargoes, "n_");
  }

  DISALLOW_COPY_AND_ASSIGN(ATSFileGenerator);
};

#endif  // PETRINET_ATS_FILE_GENERATOR_H_
