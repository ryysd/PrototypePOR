#include <iostream>
#include <vector>
#include "./pnml_parser.h"

int main() {
  PNMLParser parser;
  std::vector<Place*> places;
  std::vector<Transition*> transitions;
  parser.Parse("../benchmark/HouseCOnstruction-005.pnml", &places, &transitions);
  // parser.Parse("../benchmark/neoelection-2.unf.pnml", &places, &transitions);
  // parser.Parse("../benchmark/FMS-2.pnml", &places, &transitions);

  std::vector<std::unique_ptr<State>> states;
  Petrinet petrinet(places, transitions);
  petrinet.Execute(&states, [](const Transition* transition, const State* state) {});

  std::cout << states.size() << std::endl;

  for (auto place : places) delete place;
  for (auto transition : transitions) delete transition;
}
