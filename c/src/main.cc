/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include "../thirdparty/prettyprint.h"
#include "./atS_file_reader.h"

int main() {
  // auto pair = ATSFileReader::Read("../tmp/Philosophers-10/ats.json");
  // auto pair = ATSFileReader::Read("../tmp/sample_small/ats.json");
  auto pair = ATSFileReader::Read("../tmp/Philosopher_02/ats.json");
  pair.first->dump();
}
