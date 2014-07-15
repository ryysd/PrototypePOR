/** 
 *  @file     main.cpp  
 *  @brief    main file 
*/

#include <iostream>
#include <vector>
#include "../thirdparty/prettyprint.h"
#include "./atS_file_reader.h"

int main() {
  ATSFileReader::Read("../c/test/test_data/Philosopher_02.ats.json");
}
