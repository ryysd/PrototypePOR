/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include "../thirdparty/prettyprint.h"
#include "./atS_file_reader.h"

int main() {
  auto pair = ATSFileReader::Read("../tmp/FMS-2/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/Philosophers-5/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/Philosophers-10/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/sample_medium/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/sample/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/sample_small/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/Philosopher_02/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/interleave_3/ats.json");
  // pair.first->dump();
  std::cout << "// " << pair.first->states().size() << std::endl;
}
