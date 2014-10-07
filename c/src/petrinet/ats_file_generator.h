#ifndef PETRINET_ATS_FILE_GENERATOR_H_
#define PETRINET_ATS_FILE_GENERATOR_H_

#include <map>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "../util/simple_json.h"
#include "../util/debug.h"
#include "../util/stoi.h"
#include "../util/simple_timer.h"

class ATSFileGenerator {
 public:
  ATSFileGenerator() {}

  std::string Generate(const std::string& file_name) const {
    PNMLParser parser;
    std::vector<Place*> places;
    std::vector<Transition*> transitions;

    dprint_info("parsing pnml file...\n"); simpletimer_start();
    parser.Parse(file_name, &places, &transitions);
    dprint_info("complete. (%d ms)\n\n", simpletimer_stop());

    std::vector<std::unique_ptr<State>> states;
    Petrinet petrinet(places, transitions);

    dprint_info("executing petrinet...\n"); simpletimer_start();
    std::vector<std::tuple<const State*, const Transition*, const State*>> relations;
    auto callback = [&relations](const State* source, const Transition* transition, const State* target) { relations.push_back(std::make_tuple(source, transition, target)); };
    State* init_state = petrinet.Execute(&states, callback);
    dprint_info("%d states are found. (%d ms)\n\n", states.size(), simpletimer_stop());

    dprint_info("generating json...\n"); simpletimer_start();
    std::string json = GenerateJSON(init_state, relations, places);
    dprint_info("complete. (%d ms)\n\n", simpletimer_stop());

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
    std::unordered_map<std::string, std::vector<std::tuple<Place*, int>>> loopback_places;
    std::vector<std::unique_ptr<Action>> actions;
    Entity init_entities;
    Entity empty_vector;

    for (auto relation : relations) {
      const State* source = std::get<0>(relation);
      const Transition* transition = std::get<1>(relation);
      const State* target = std::get<2>(relation);

      Entity source_entities, target_entities;
      Entity source_guard_entities, target_guard_entities;
      CreateEntities(source->marking(), places, &source_entities, &source_guard_entities);
      CreateEntities(target->marking(), places, &target_entities, &target_guard_entities);

      if (source->Equals(*init_state) && init_entities.empty()) {
        init_entities = source_guard_entities;
        init_entities.insert(init_entities.end(), source_entities.begin(), source_entities.end());
      }

      if (loopback_places.find(transition->id()) == loopback_places.end()) {
        CalcLoopbackPLaces(places, transition, &loopback_places[transition->id()]);
      }

      Entity creator, eraser, reader, embargoes;
      CreateReader(loopback_places.at(transition->id()), &reader);
      CreateCreatorEraser(source_entities, source_guard_entities, target_entities, target_guard_entities, reader, &creator, &eraser);
      CreateEmbargoes(eraser, &embargoes);

      std::string entity_string;
      MakeEntityString(creator, eraser, empty_vector, empty_vector, &entity_string);
      std::string action_entities_name = transition->id() + "_" + entity_string;
      if (name_map.find(action_entities_name) == name_map.end()) {
        appearance_counts[transition->id()] += 1;
        std::string action_name = transition->id() + "_" + std::to_string(appearance_counts[transition->id()]);
        actions.push_back(std::unique_ptr<Action>(new Action{action_name, creator, reader, eraser, embargoes}));
        name_map.insert(std::make_pair(action_entities_name, action_name));
      }
    }

    if (!loopback_places.empty()) dprint_warn("%d loopback was found. Result is may be incorrect.\n", loopback_places.size());

    std::map<std::string, std::string> hash;
    hash.insert(std::make_pair("actions", MakeActionsJson(actions)));
    hash.insert(std::make_pair("lts", MakeLtsJson(init_entities)));

    return simplejson::make_json_document(simplejson::make_json_hash(hash));
  }

  void CalcLoopbackPLaces(const std::vector<Place*>& places, const Transition* transition, std::vector<std::tuple<Place*, int>>* loopbacks) const {
    for (auto place : places) {
      const Arc* source_arc = place->FindArc(transition);
      const Arc* target_arc = transition->FindArc(place);
      if (source_arc && target_arc && source_arc->inscription() == target_arc->inscription()) {
        loopbacks->push_back(std::make_tuple(place, source_arc->inscription()));
      }
    }
  }

  void CreateReader(const std::vector<std::tuple<Place*, int>>& loopbacks, Entity* reader) const {
    for (auto& loopback : loopbacks) {
      for (int i = 1, n = std::get<1>(loopback); i <= n; ++i) {
        reader->push_back(std::get<0>(loopback)->id() + '_' + std::to_string(i));
      }
    }
    // std::transform(loopbacks.begin(), loopbacks.end(), std::back_inserter(*reader), [](const Place* place) { return place->id() + '_' + std::to_string(i); });
  }

  void CreateEmbargoes(const Entity& eraser, Entity* embargoes) const {
    std::unordered_map<std::string, int> maxes;

    for (auto& entity : eraser) {
      std::vector<std::string> splitted;
      split(entity, '_', &splitted);
      int num = stoi(splitted.back());
      std::string entity_name = "";
      for (int i = 0, n = splitted.size() - 1; i < n; ++i) entity_name += splitted[i];

      if (maxes.find(entity_name) != maxes.end()) {
        if (maxes[entity_name] < num) maxes[entity_name] = num;
      } else {
        maxes[entity_name] = num;
      }
    }

    for (auto& kv : maxes) {
      embargoes->push_back(kv.first + "_" + std::to_string(kv.second + 1));
    }
  }

  void CreateCreatorEraser(
      const Entity& source_entities, const Entity& source_guard_entities,
      const Entity& target_entities, const Entity& target_guard_entities,
      const Entity& reader,
      Entity* creator, Entity* eraser) const {
    Entity forbitten_creator, forbitten_eraser;
    std::set_difference(target_guard_entities.begin(), target_guard_entities.end(), source_guard_entities.begin(), source_guard_entities.end(), std::back_inserter(forbitten_creator));
    std::set_difference(source_guard_entities.begin(), source_guard_entities.end(), target_guard_entities.begin(), target_guard_entities.end(), std::back_inserter(forbitten_eraser));

    Entity real_creator, real_eraser;
    std::set_difference(target_entities.begin(), target_entities.end(), source_entities.begin(), source_entities.end(), std::back_inserter(real_creator));
    std::set_difference(source_entities.begin(), source_entities.end(), target_entities.begin(), target_entities.end(), std::back_inserter(real_eraser));

    Entity duplicate_creator = forbitten_creator;
    Entity duplicate_eraser = forbitten_eraser;
    duplicate_creator.insert(duplicate_creator.end(), real_creator.begin(), real_creator.end());
    duplicate_eraser.insert(duplicate_eraser.end(), real_eraser.begin(), real_eraser.end());

    std::set_difference(duplicate_creator.begin(), duplicate_creator.end(), reader.begin(), reader.end(), std::back_inserter(*creator));
    std::set_difference(duplicate_eraser.begin(), duplicate_eraser.end(), reader.begin(), reader.end(), std::back_inserter(*eraser));
  }

  void CreateEntities(const std::vector<int>& marking, const std::vector<Place*>& places, Entity* entities, Entity* guard_entities) const {
    for (int i = 0, n = marking.size(); i < n; ++i) {
      if (marking[i] > 0) {
        for (int j = 0, m = marking[i]; j < m; ++j) {
          entities->push_back(places[i]->id() + "_" + std::to_string(j+1));
        }
        // guard_entities->push_back("@" + places[i]->id());
      }
    }

    std::sort(entities->begin(), entities->end());
    std::sort(guard_entities->begin(), guard_entities->end());
  }

  std::string MakeActionJSON(const std::unique_ptr<Action>& action) const {
    std::map<std::string, std::string> hash;

    hash.insert(std::make_pair("c", simplejson::make_json_array(action->c)));
    hash.insert(std::make_pair("d", simplejson::make_json_array(action->d)));
    hash.insert(std::make_pair("n", simplejson::make_json_array(action->n)));
    hash.insert(std::make_pair("r", simplejson::make_json_array(action->r)));

    return simplejson::make_json_key_values(hash);
  }

  std::string MakeActionsJson(const std::vector<std::unique_ptr<Action>>& actions) const {
    std::map<std::string, std::string> hash;
    for (auto& action : actions) {
      hash.insert(std::make_pair(action->name, MakeActionJSON(action)));
    }
    return simplejson::make_json_hash("entities", simplejson::make_json_hash(hash));
  }

  std::string MakeLtsJson(const Entity& init_entities) const {
    std::map<std::string, std::string> hash;
    hash.insert(std::make_pair("init_entities", simplejson::make_json_array(init_entities)));
    return simplejson::make_json_key_values(hash);
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
