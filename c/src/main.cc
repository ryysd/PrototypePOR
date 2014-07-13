/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include "../thirdparty/prettyprint.h"
#include "./util/atS_file_reader.h"

int main() {
  ATSFileReader::Read("../tmp/sample_small/ats.json");
}
