#ifndef PETRINET_PETRINET_H_
#define PETRINET_PETRINET_H_

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include "../util/macro.h"
#include "./arc.h"
#include "./petrinet_node.h"
#include "./transition.h"
#include "./place.h"
#include "./state.h"

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

  DISALLOW_COPY_AND_ASSIGN(Petrinet);
};

#endif  // PETRINET_PETRINET_H_
