#include <iostream>
#include <vector>
#include "./pnml_parser.h"

int main() {
  PNMLParser parser;
  std::vector<PlacePtr> places;
  std::vector<TransitionPtr> transitions;
  parser.Parse("../benchmark/Philosopher_02.xml", &places, &transitions);
}
