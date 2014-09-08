/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include "../thirdparty/prettyprint.h"
#include "../thirdparty/cmdline.h"
#include "./ats_file_reader.h"
#include "./reducer/probe_reducer.h"
#include "./reducer/validation_checker.h"

int main(int argc, char** argv) {
  cmdline::parser parser;

  parser.add<std::string>("ats", 'a');
  parser.add<std::string>("dot", 'd', "", false);
  parser.add<bool>("hide-state-name", 's', "", false);
  parser.add<bool>("hide-edge-name", 'e', "", false);

  if (!parser.parse(argc, argv)) {
    std::cout << parser.error_full() << parser.usage();
    return 0;
  }

  auto pair = ATSFileReader::Read(parser.get<std::string>("ats"));
  // auto pair = ATSFileReader::Read("../tmp/FMS-2/ats.json");
  // pair.first->dump();
  std::cout << "// " << pair.first->states().size() << std::endl;

  std::unordered_map<std::string, const State*> visited_states;
  std::unordered_map<std::string, const Action*> executed_actions;
  ProbeReducer* reducer = new ProbeReducer(pair.second);
  reducer->Reduce(pair.first->init_state(), &visited_states, &executed_actions);

  std::cout << executed_actions.size() << std::endl;
  std::cout << pair.second->actions().size() << std::endl;
  std::cout << executed_actions << std::endl;
  std::cout << pair.second->actions() << std::endl;

  int full_size = pair.first->states().size();
  int reduced_size = visited_states.size();
  float reduction_ratio = static_cast<float>(full_size - reduced_size)*100 / full_size;

  std::cout << full_size << " -> " << reduced_size << "(" << reduction_ratio << "%)" << std::endl;
  ValidationChecker::DumpValidation(pair.first, visited_states);
  if (parser.exist("dot")) {
    std::string dot_file_name = parser.get<std::string>("dot");
    std::ofstream ofs(dot_file_name);
    if (ofs.fail()) {
      ERROR("cannot open %s.", dot_file_name.c_str());
      return 0;
    }

    bool display_state_name = !parser.exist("hide-state-name") || !parser.get<bool>("hide-state-name");
    bool display_edge_name = !parser.exist("hide-edge-name") || !parser.get<bool>("hide-edge-name");
    ofs << pair.first->ToDot(visited_states, display_state_name, display_edge_name);
  }
}
