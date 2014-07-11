/** 
 *  @file     ats_file_reader.hpp  
 *  @brief    File containing reader of ATS file
 */

#ifndef UTIL_ATS_FILE_READER_H_
#define UTIL_ATS_FILE_READER_H_

#include <string>
#include <fstream>
#include "picojson.h"
#include "logger.h"

class ATSFileReader {
  public:
    static void* Read(std::string fname) {
      std::ifstream ifs(fname);

      if (ifs.fail()) {
        ERROR("cannot open %s", fname.c_str());
	return NULL;
      }

      std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
      return NULL;
    }
};

#endif
