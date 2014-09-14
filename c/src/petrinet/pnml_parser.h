#ifndef PETRINET_PNML_PARSER_H_
#define PETRINET_PNML_PARSER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <stack>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "../../thirdparty/rapidxml/rapidxml.hpp"

// #include <iomanip>
// #include "../por_module/util/profiler.h"

class PetrinetNode;
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

class PetrinetNode {
 public:
  PetrinetNode(const std::string& id, const std::string& name)
    : id_(id), name_(name) {}

  void AddArc(Arc* arc) const { arcs_.push_back(std::unique_ptr<Arc>(arc)); }
  const Arc* FindArc(const PetrinetNode* node) const {
    auto result = std::find_if(arcs_.begin(), arcs_.end(), [node](const std::unique_ptr<Arc>& arc) { return arc->target()->Equals(*node); });
    return (result != arcs_.end()) ? result->get() : nullptr;
  }

  bool Equals(const PetrinetNode& other) const { return id_ == other.id(); }

  const std::string& id() const { return id_; }
  const std::string& name() const { return name_; }
  const std::vector<std::unique_ptr<Arc>>& arcs() const { return arcs_; }
 private:
  const std::string id_;
  const std::string name_;
  mutable std::vector<std::unique_ptr<Arc>> arcs_;
};

class Place : public PetrinetNode {
 public:
  Place(const std::string& id, const std::string& name, int initial_marking)
  : PetrinetNode(id, name), initial_marking_(initial_marking) {}

  int initial_marking() const { return initial_marking_; }
 private:
  const int initial_marking_;
};

class Transition : public PetrinetNode {
 public:
  Transition(const std::string& id, const std::string& name)
    : PetrinetNode(id, name) {}
};

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
};

typedef std::vector<std::vector<int>> Matrix;
class State {
 public:
  explicit State(const std::vector<int>& marking) : marking_(marking), marking_ptr_(&(marking_.front())), marking_size(marking_.size()) { MakeHash(); }
  State(const int* marking, int marking_length) : marking_(marking, marking + marking_length), marking_ptr_(&(marking_.front())), marking_size(marking_.size()) { MakeHash(); }

  // TODO(ryysd): optimize
  void CalcSuccessors(const Matrix& incidence_column_vectors, const std::vector<Transition*>& transitions, std::map<Transition*, State*>* successors) const {
    int* new_marking_ptr = new int[marking_.size()];
    bool valid = true;

    for (int i = 0, n = transitions.size(); i < n; ++i) {
      const int* incidence_column_vector_ptr = &(incidence_column_vectors[i].front());

      for (int j = 0, m = marking_.size(); j < m; ++j) {
        int sum = marking_ptr_[j] + incidence_column_vector_ptr[j];
        if ((valid = sum >= 0)) new_marking_ptr[j] = sum; else break;  // NOLINT
      }

      if (valid) successors->insert(std::make_pair(transitions[i], new State(new_marking_ptr, marking_.size())));
    }

    delete new_marking_ptr;
  }

  bool Equals(const State& other) const { return std::memcmp(marking_ptr_, other.marking_ptr(), sizeof(int) * marking_.size()) == 0; }
  const std::vector<int>& marking() const { return marking_; }
  const int* marking_ptr() const { return marking_ptr_; }
  const std::string& hash() const { return hash_; }

 private:
  void MakeHash() {
    hash_ =  "[";
    for (auto i : marking_) hash_ += std::to_string(i) + ",";
    if (!hash_.empty()) hash_.erase( --hash_.end() );
    hash_ += "]";
  }

  const std::vector<int> marking_;
  const int* marking_ptr_;
  const int marking_size;
  std::string hash_;
};

class Petrinet {
 public:
  Petrinet(const std::vector<Place*> places, const std::vector<Transition*> transitions) : places_(places), transitions_(transitions) {
    std::vector<int> initial_marking;
    std::transform(places.begin(), places.end(), std::back_inserter(initial_marking), [](const Place* place) { return place->initial_marking(); });

    CreateMatrix(&input_matrix_, &output_matrix_, &incidence_matrix_);
  }

  template <class ExecuteCallback>
  State* Execute(std::vector<std::unique_ptr<State>>* states, ExecuteCallback callback) const {
    std::vector<int> initial_marking;
    std::transform(places_.begin(), places_.end(), std::back_inserter(initial_marking), [](const Place* place) { return place->initial_marking(); });
    std::stack<State*> stack;

    State* init_state = new State(initial_marking);
    stack.push(init_state);

    Matrix column_vectors;
    CalcColumnVectors(places_.size(), transitions_.size(), incidence_matrix_, &column_vectors);

    std::unordered_map<std::string, State*> states_map;

    std::map<Transition*, State*> successors;
    while (!stack.empty()) {
      State* state = stack.top();
      stack.pop();

      if (states_map.find(state->hash()) != states_map.end()) continue;  // NOLINT
      states_map.insert(std::make_pair(state->hash(), state));

      if (states_map.size() % 10000 == 0) std::cout << states_map.size() << std::endl;

      successors.clear();
      state->CalcSuccessors(column_vectors, transitions_, &successors);

      for (auto kv : successors) {
        callback(state, kv.first, kv.second);
        stack.push(kv.second);
      }
    }

    std::transform(states_map.begin(), states_map.end(), std::back_inserter(*states), [](const std::unordered_map<std::string, State*>::value_type& kv) { return std::unique_ptr<State>(kv.second); });

    return init_state;
  }

 private:
  void CreateMatrix(Matrix* input_matrix, Matrix* output_matrix, Matrix* incidence_matrix) const {
    int row_size = places_.size();
    int col_size = transitions_.size();
    *input_matrix = Matrix(row_size);
    *output_matrix = Matrix(row_size);
    *incidence_matrix = Matrix(row_size);

    for (auto& matrix : *input_matrix) matrix = std::vector<int>(col_size);
    for (auto& matrix : *output_matrix) matrix = std::vector<int>(col_size);
    for (auto& matrix : *incidence_matrix) matrix = std::vector<int>(col_size);

    for (int i = 0, n = places_.size(); i < n; ++i) {
      const Place* place = places_[i];
      for (int j = 0, m = transitions_.size(); j < m; ++j) {
        const Transition* transition = transitions_[j];

        const Arc* input_arc = transition->FindArc(place);
        const Arc* output_arc = place->FindArc(transition);

        (*input_matrix)[i][j] = input_arc ? input_arc->inscription() : 0;
        (*output_matrix)[i][j] = output_arc ? output_arc->inscription() : 0;
        (*incidence_matrix)[i][j] = (*input_matrix)[i][j] - (*output_matrix)[i][j];
      }
    }
  }

  void CalcColumnVectors(int row_size, int col_size, const Matrix& matrix, Matrix* column_vectors) const {
    *column_vectors = Matrix(col_size);

    for (int i = 0; i < col_size; ++i) {
      (*column_vectors)[i] = std::vector<int>(row_size);
      for (int j = 0; j < row_size; ++j) {
        (*column_vectors)[i][j] = matrix[j][i];
      }
    }
  }

  void DumpMatrix(const Matrix& input_matrix, const Matrix& output_matrix, const Matrix& incidence_matrix) const {
    for (auto row_vector : input_matrix) { for (auto column : row_vector) { std::cout << column << ","; } std::cout << std::endl; }
    for (auto row_vector : output_matrix) { for (auto column : row_vector) { std::cout << column << ","; } std::cout << std::endl; }
    for (auto row_vector : incidence_matrix) { for (auto column : row_vector) { std::cout << column << ","; } std::cout << std::endl; }

    for (auto transition : transitions_) std::cout << transition->id() << ",";
    std::cout << std::endl;
    for (auto place : places_) std::cout << place->id() << std::endl;
  }

  const std::vector<Place*> places_;
  const std::vector<Transition*> transitions_;
  Matrix input_matrix_;
  Matrix output_matrix_;
  Matrix incidence_matrix_;
};

#endif  // PETRINET_PNML_PARSER_H_
