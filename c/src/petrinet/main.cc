#include <iostream>
#include "./pnml_parser.h"
#include "./petrinet_option.h"
#include "./ats_file_generator.h"

int main(int argc, char** argv) {
  PetrinetOption option;
  option.Parse(argc, argv);

  ATSFileGenerator generator;
  generator.Generate(option.pnml_file_name());

  // PNMLParser parser;
  // std::vector<Place*> places;
  // std::vector<Transition*> transitions;
  // parser.Parse(option.pnml_file_name(), &places, &transitions);

  // std::vector<std::unique_ptr<State>> states;
  // Petrinet petrinet(places, transitions);
  // petrinet.Execute(&states, [](const Transition* transition, const State* state) {});

  // std::cout << states.size() << std::endl;

  // for (auto place : places) delete place;
  // for (auto transition : transitions) delete transition;
}
