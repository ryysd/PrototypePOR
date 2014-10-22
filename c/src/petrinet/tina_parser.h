#ifndef PETRINET_TINA_PARSER_H_
#define PETRINET_TINA_PARSER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <regex>  // NOLINT
#include "./../util/stoi.h"
#include "./../util/debug.h"

// TinaParser does not support weighted arc.
class TinaParser {
 public:
  TinaParser() {}

  typedef struct _TransitionData {const std::string name; const std::string input_places; const std::string output_places;} TransitionData;

  template <class ExecuteCallback>
  void Parse(const std::string& file_name, std::vector<Place*>* places, std::vector<Transition*>* transitions, std::vector<std::unique_ptr<State>>* states, ExecuteCallback callback) const {
    std::unordered_map<std::string, int> place_indexes;

    std::ifstream ifs(file_name);

    ParsePlaceTransition(&ifs, places, transitions, &place_indexes);

    dprint("%d transitions, %d places.\n", transitions->size(), places->size());

    ParseMarkings(places->size(), place_indexes, &ifs, states);
    ParseReachabilityGraph(*transitions, *states, &ifs, callback);
  }

 private:
  template <class ExecuteCallback>
  void ParseReachabilityGraph(const std::vector<Transition*>& transitions, const std::vector<std::unique_ptr<State>>& states, std::ifstream* ifs, ExecuteCallback callback) const {
    std::regex action_pattern("^(\\d+) -> (.*)$");

    std::string line;
    while (getline(*ifs, line)) {
      std::smatch match;
      if (std::regex_match(line, match, action_pattern)) {
        std::vector<std::string> actions;
        split(match[2].str(), ',', &actions);
        int state_id = stoi(match[1].str());

        for (int i = 0, n = actions.size(); i < n; ++i) {
          const std::string& action = trim(actions[i]);

          if (action.empty()) continue;

          std::vector<std::string> action_detail;
          split(action, '/', &action_detail);

          assert(action_detail.size() >= 2);

          std::string action_name = "";
          unsigned int successor_id = stoi(action_detail.back());
          for (int i = 0, n = action_detail.size() - 1; i < n; ++i) action_name += action_detail[i] + "/";
          action_name.erase(--action_name.end());

          assert(successor_id < states.size());

          const State* state = states[state_id].get();
          const auto transition_it = std::find_if(transitions.begin(), transitions.end(), [action_name](const Transition* transition) { return transition->name() == action_name; });
          const State* successor = states[successor_id].get();

          assert(transition_it != transitions.end());

          callback(state, *transition_it, successor);
        }
      } else if (line.find("ANALYSIS COMPLETED") != std::string::npos) {
        break;
      }
    }
  }

  void ParseMarkings(int marking_size, const std::unordered_map<std::string, int>& place_indexes, std::ifstream* ifs, std::vector<std::unique_ptr<State>>* states) const {
    std::regex marking_pattern("^\\d+ : (.*)$");

    std::string line;
    while (getline(*ifs, line)) {
      std::smatch match;
      if (std::regex_match(line, match, marking_pattern)) {
        std::vector<int> marking(marking_size, 0);
        std::vector<std::string> token_infos;

        split(trim(match[1].str()), ' ', &token_infos);
        for (const auto& token_info : token_infos) {
          std::vector<std::string> token_detail;
          split(token_info, '*', &token_detail);
          std::string token_name = token_detail[0];
          int token_num = token_detail.size() == 1 ? 1 : stoi(token_detail[1].c_str());

          marking[place_indexes.at(token_name)] = token_num;
        }

        states->push_back(std::unique_ptr<State>(new State(marking)));
      } else if (!states->empty()) {
        break;
      }
    }
  }

  void ParsePlaceTransition(std::ifstream* ifs, std::vector<Place*>* places, std::vector<Transition*>* transitions, std::unordered_map<std::string, int>* place_indexes) const {
    std::regex transition_pattern("^tr (\\S+) (.*) -> (.*)$");
    std::regex place_pattern("^pl (\\S+) \\((\\d+)\\)$");
    std::regex net_pattern("^net (.*)$");

    std::vector<TransitionData> transition_data;

    std::string line;
    bool section_started = false;
    while (getline(*ifs, line)) {
      std::smatch match;
      if (std::regex_match(line, match, transition_pattern)) {
        transition_data.push_back(TransitionData{trim(match[1].str()), trim(match[2].str()), trim(match[3].str())});
      } else if (std::regex_match(line, match, place_pattern)) {
        InsertPlace(new Place(match[1].str(), match[1].str(), stoi(match[2].str().c_str())), places, place_indexes);
      } else if (std::regex_match(line, match, net_pattern)) {
        section_started = true;
      } else if (section_started) {
        break;
      }
    }

    InsertNewNodes(transition_data, transitions, places, place_indexes);
  }

  void InsertNewNodes(const std::vector<TransitionData>& transition_data,
      std::vector<Transition*>* transitions, std::vector<Place*>* places,
      std::unordered_map<std::string, int>* place_indexes) const {
    for (auto& data : transition_data) {
      Transition* transition = CreateTransition(data, transitions);

      std::vector<std::string> input_places, output_places;
      split(data.input_places, ' ', &input_places);
      split(data.output_places, ' ', &output_places);

      for (auto& place_name : input_places) {
        Place* place = CreatePlace(place_name, places, place_indexes);
        place->AddArc(new Arc(place, transition, 1));
      }

      for (auto& place_name : output_places) {
        Place* place = CreatePlace(place_name, places, place_indexes);
        transition->AddArc(new Arc(transition, place, 1));
      }
    }
  }

  Transition* CreateTransition(const TransitionData& data, std::vector<Transition*>* transitions) const {
    auto transition_it = std::find_if(transitions->begin(), transitions->end(), [&data](const Transition* transition) { return transition->name() == data.name; });

    if (transition_it != transitions->end()) return *(transition_it);

    transitions->push_back(new Transition(data.name, data.name));
    return transitions->back();
  }

  Place* CreatePlace(const std::string name, std::vector<Place*>* places, std::unordered_map<std::string, int>* place_indexes) const {
    auto place_it = std::find_if(places->begin(), places->end(), [&name](const Place* place) { return place->name() == name; });

    if (place_it != places->end()) return *(place_it);

    InsertPlace(new Place(name, name, 0), places, place_indexes);
    return places->back();
  }

  void InsertPlace(Place* place, std::vector<Place*>* places, std::unordered_map<std::string, int>* place_indexes) const {
    place_indexes->insert(std::make_pair(place->name(), places->size()));
    places->push_back(place);
  }

  DISALLOW_COPY_AND_ASSIGN(TinaParser);
};

#endif  // PETRINET_TINA_PARSER_H_
