#ifndef PETRINET_PETRINET_OPTION_H_
#define PETRINET_PETRINET_OPTION_H_

#include <string>
#include "../../thirdparty/cmdline.h"

class PetrinetOption {
 public:
  PetrinetOption() {}

  bool Parse(int argc, char** argv) {
    parser_.add<std::string>("file", 'f');
    parser_.add<std::string>("out", 'o');
    parser_.add("tina", 't');
    parser_.add("debug", 'd');

    if (!parser_.parse(argc, argv)) {
      std::cout << parser_.error_full() << parser_.usage();
      return false;
    }

    return true;
  }

  std::string file_name() const { return parser_.exist("file") ? std::move(parser_.get<std::string>("file")) : ""; }
  std::string output_file_name() const { return parser_.exist("out") ? std::move(parser_.get<std::string>("out")) : ""; }
  bool debug() const { return parser_.exist("debug"); }
  bool tina() const { return parser_.exist("tina"); }

 private:
  cmdline::parser parser_;

  DISALLOW_COPY_AND_ASSIGN(PetrinetOption);
};

#endif  // PETRINET_PETRINET_OPTION_H_
