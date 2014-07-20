/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include <string>
#include "../thirdparty/prettyprint.h"
#include "../thirdparty/cmdline.h"
#include "./atS_file_reader.h"

int main(int argc, char** argv) {
  cmdline::parser parser;

  parser.add<std::string>("ats", 'a');
  if (!parser.parse(argc, argv)) {
    std::cout << parser.error_full() << parser.usage();
  }

  auto pair = ATSFileReader::Read(parser.get<std::string>("ats"));
  // auto pair = ATSFileReader::Read("../tmp/FMS-2/ats.json");
  // pair.first->dump();
  std::cout << "// " << pair.first->states().size() << std::endl;
}
