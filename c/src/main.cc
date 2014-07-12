/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include "../thirdparty/prettyprint.h"
#include "./util/atS_file_reader.h"

int main() {
  ATSFileReader::Read("../tmp/Philosophers-10/ats.json");
}
