#ifndef PETRINET_PNML_PARSER_H_
#define PETRINET_PNML_PARSER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "../../thirdparty/rapidxml/rapidxml.hpp"

class Arc;
class PetrinetNode {
 public:
  PetrinetNode(const std::string& id, const std::string& name)
    : id_(id), name_(name) {}

  void AddArc(Arc* arc) const { arcs_.push_back(std::unique_ptr<Arc>(arc)); }

  const std::string& id() const { return id_; }
  const std::string& name() const { return name_; }
  const std::vector<std::unique_ptr<Arc>>& arcs() const { return arcs_; }
 private:
  const std::string id_;
  const std::string name_;
  mutable std::vector<std::unique_ptr<Arc>> arcs_;
};

class Arc {
 public:
  Arc(const PetrinetNode* source, const PetrinetNode* target, int inscription)
    : source_(source), target_(target), inscription_(inscription) {}

  const PetrinetNode* source() const { return source_; }
  const PetrinetNode* target() const { return target_; }
  int inscription() const { return inscription_; }

 private:
  const PetrinetNode* source_;
  const PetrinetNode* target_;
  const int inscription_;
};

class Place : public PetrinetNode {
 public:
  Place(const std::string& id, const std::string& name, int initial_marking)
  : PetrinetNode(id, name), initial_marking_(initial_marking) {}

  int initial_marking() const { return initial_marking_; }
 private:
  const int initial_marking_;
};
typedef std::unique_ptr<Place> PlacePtr;

class Transition : public PetrinetNode {
 public:
  Transition(const std::string& id, const std::string& name)
    : PetrinetNode(id, name) {}
};
typedef std::unique_ptr<Transition> TransitionPtr;

int stoi(const char* string) {
  int result = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    if (isdigit(string[i])) result = result * 10 + (string[i] - '0');
  }

  return result;
}

class PNMLParser {
 public:
  typedef std::vector<const rapidxml::xml_node<>*> xml_nodes;

  void Parse(const std::string& file_name, std::vector<PlacePtr>* places, std::vector<TransitionPtr>* transitions) const {
    std::ifstream ifs(file_name);
    std::vector<char> xml_data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    xml_data.push_back('\0');

    rapidxml::xml_document<> doc;
    doc.parse<0>(&xml_data[0]);

    rapidxml::xml_node<>* net = doc.first_node("pnml")->first_node("net");
    bool is_pipe_form = strcmp(net->first_attribute("type")->value(), "P/T net") == 0;
    std::string value_key = is_pipe_form ? "value" : "text";
    rapidxml::xml_node<>* base = is_pipe_form ? net : net->first_node("page");

    std::vector<const rapidxml::xml_node<>*> place_nodes, transition_nodes, arc_nodes;
    for (rapidxml::xml_node<>* node  = base->first_node(); node; node = node->next_sibling()) {
      if (strcmp(node->name(), "place") == 0) place_nodes.push_back(node);
      else if (strcmp(node->name(), "transition") == 0) transition_nodes.push_back(node);
      else if (strcmp(node->name(), "arc") == 0) arc_nodes.push_back(node);
      else std::cout << "unknown node is found: " << node->name() << std::endl;  // NOLINT
    }

    ParsePlaces(value_key, place_nodes, places);
    ParseTransitions(value_key, transition_nodes, transitions);
    ParseArcs(value_key, arc_nodes, places, transitions);
  }

 private:
  void ParsePlaces(const std::string& value_key, const xml_nodes& place_nodes, std::vector<PlacePtr>* places) const {
    for (auto place_node : place_nodes) {
      rapidxml::xml_attribute<>* id = place_node->first_attribute("id");
      rapidxml::xml_node<>* name = place_node->first_node("name");
      rapidxml::xml_node<>* initial_marking = place_node->first_node("initialMarking");
      Place* place = new Place(
          id->value(),
          name ? name->first_node(value_key.c_str())->value() : id->value(),
          initial_marking ? stoi(initial_marking->first_node(value_key.c_str())->value()) : 0);

      places->push_back(PlacePtr(place));
    }
  }

  void ParseTransitions(const std::string& value_key, const xml_nodes& transition_nodes, std::vector<TransitionPtr>* transitions) const {
    for (auto transition_node : transition_nodes) {
      rapidxml::xml_attribute<>* id = transition_node->first_attribute("id");
      rapidxml::xml_node<>* name = transition_node->first_node("name");
      Transition* transition = new Transition(
          id->value(),
          name ? name->first_node(value_key.c_str())->value() : id->value());

      transitions->push_back(TransitionPtr(transition));
    }
  }

  void ParseArcs(const std::string& value_key, const xml_nodes& arc_nodes, std::vector<PlacePtr>* places, std::vector<TransitionPtr>* transitions) const {
    for (auto arc_node : arc_nodes) {
      const char* source_id = arc_node->first_attribute("source")->value();
      const char* target_id = arc_node->first_attribute("target")->value();

      auto source_place = std::find_if(places->begin(), places->end(), [source_id](const PlacePtr& node) { return node->id().compare(source_id) == 0; });
      auto target_place = std::find_if(places->begin(), places->end(), [target_id](const PlacePtr& node) { return node->id().compare(target_id) == 0; });

      auto source_transition = std::find_if(transitions->begin(), transitions->end(), [source_id](const TransitionPtr& node) { return node->id().compare(source_id) == 0; });
      auto target_transition = std::find_if(transitions->begin(), transitions->end(), [target_id](const TransitionPtr& node) { return node->id().compare(target_id) == 0; });

      rapidxml::xml_node<>* inscription_node = arc_node->first_node("inscription");
      int inscription = inscription_node ? stoi(inscription_node->first_node(value_key.c_str())->value()) : 1;
      Arc* arc = (source_place != places->end()) ?  new Arc(source_place->get(), target_transition->get(), inscription) : new Arc(source_transition->get(), target_place->get(), inscription);

      (source_place != places->end()) ? (*source_place)->AddArc(arc) : (*source_transition)->AddArc(arc);
    }
  }
};

#endif  // PETRINET_PNML_PARSER_H_
