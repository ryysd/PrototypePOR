#ifndef PETRINET_PNML_PARSER_H_
#define PETRINET_PNML_PARSER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "../../thirdparty/rapidxml/rapidxml.hpp"
#include "./../util/stoi.h"
#include "./../util/debug.h"
#include "./petrinet.h"

// #include <iomanip>
// #include "../por_module/util/profiler.h"

class PNMLParser {
 public:
  typedef std::vector<const rapidxml::xml_node<>*> xml_nodes;

  PNMLParser() {}

  void Parse(const std::string& file_name, std::vector<Place*>* places, std::vector<Transition*>* transitions) const {
    std::ifstream ifs(file_name);
    std::vector<char> xml_data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    xml_data.push_back('\0');

    rapidxml::xml_document<> doc;

    try {
      doc.parse<0>(&xml_data[0]);
    } catch (rapidxml::parse_error& err) {
      std::cout << err.what() << " " << err.where<char*>() << std::endl;
      return;
    }

    rapidxml::xml_node<>* net = doc.first_node("pnml")->first_node("net");
    bool is_pipe_form = strcmp(net->first_attribute("type")->value(), "P/T net") == 0;
    std::string value_key = is_pipe_form ? "value" : "text";
    rapidxml::xml_node<>* base = is_pipe_form ? net : net->first_node("page");

    std::vector<const rapidxml::xml_node<>*> place_nodes, transition_nodes, arc_nodes;
    for (rapidxml::xml_node<>* node  = base->first_node(); node; node = node->next_sibling()) {
      if (strcmp(node->name(), "place") == 0) { place_nodes.push_back(node); }  // NOLINT
      else if (strcmp(node->name(), "transition") == 0) {transition_nodes.push_back(node); }  // NOLINT
      else if (strcmp(node->name(), "arc") == 0) { arc_nodes.push_back(node); }  // NOLINT
      else if (strcmp(node->name(), "token") == 0) {}  // NOLINT
      else if (strcmp(node->name(), "name") == 0) {}  // NOLINT
      else { std::cout << "unknown node is found: " << node->name() << std::endl; }  // NOLINT
    }

    ParsePlaces(value_key, place_nodes, places);
    ParseTransitions(value_key, transition_nodes, transitions);
    dprint_info("%d places, %d transitions.\n", places->size(), transitions->size());
    ParseArcs(value_key, arc_nodes, places, transitions);
  }

 private:
  void ParsePlaces(const std::string& value_key, const xml_nodes& place_nodes, std::vector<Place*>* places) const {
    for (auto place_node : place_nodes) {
      rapidxml::xml_attribute<>* id = place_node->first_attribute("id");
      rapidxml::xml_node<>* name = place_node->first_node("name");
      rapidxml::xml_node<>* initial_marking = place_node->first_node("initialMarking");
      Place* place = new Place(
          id->value(),
          name ? name->first_node(value_key.c_str())->value() : id->value(),
          initial_marking ? stoi(initial_marking->first_node(value_key.c_str())->value()) : 0);

      places->push_back(place);
    }
  }

  void ParseTransitions(const std::string& value_key, const xml_nodes& transition_nodes, std::vector<Transition*>* transitions) const {
    for (auto transition_node : transition_nodes) {
      rapidxml::xml_attribute<>* id = transition_node->first_attribute("id");
      rapidxml::xml_node<>* name = transition_node->first_node("name");
      Transition* transition = new Transition(
          id->value(),
          name ? name->first_node(value_key.c_str())->value() : id->value());

      transitions->push_back(transition);
    }
  }

  void ParseArcs(const std::string& value_key, const xml_nodes& arc_nodes, std::vector<Place*>* places, std::vector<Transition*>* transitions) const {
    for (auto arc_node : arc_nodes) {
      const char* source_id = arc_node->first_attribute("source")->value();
      const char* target_id = arc_node->first_attribute("target")->value();

      auto source_place = std::find_if(places->begin(), places->end(), [source_id](const Place* node) { return node->id().compare(source_id) == 0; });
      auto target_place = std::find_if(places->begin(), places->end(), [target_id](const Place* node) { return node->id().compare(target_id) == 0; });

      auto source_transition = std::find_if(transitions->begin(), transitions->end(), [source_id](const Transition* node) { return node->id().compare(source_id) == 0; });
      auto target_transition = std::find_if(transitions->begin(), transitions->end(), [target_id](const Transition* node) { return node->id().compare(target_id) == 0; });

      rapidxml::xml_node<>* inscription_node = arc_node->first_node("inscription");
      int inscription = inscription_node ? stoi(inscription_node->first_node(value_key.c_str())->value()) : 1;
      Arc* arc = (source_place != places->end()) ?  new Arc(*source_place, *target_transition, inscription) : new Arc(*source_transition, *target_place, inscription);

      (source_place != places->end()) ? (*source_place)->AddArc(arc) : (*source_transition)->AddArc(arc);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(PNMLParser);
};

#endif  // PETRINET_PNML_PARSER_H_
