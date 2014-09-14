#ifndef PETRINET_PETRINET_OPTION_H_
#define PETRINET_PETRINET_OPTION_H_

#include <string>
#include "../../thirdparty/cmdline.h"

class PetrinetOption {
 public:
  PetrinetOption() {}

  bool Parse(int argc, char** argv) {
    parser_.add<std::string>("pnml", 'p');

    if (!parser_.parse(argc, argv)) {
      std::cout << parser_.error_full() << parser_.usage();
      return false;
    }

    return true;
  }

  std::string pnml_file_name() const { return parser_.exist("pnml") ? std::move(parser_.get<std::string>("pnml")) : ""; }
 private:
  cmdline::parser parser_;

  // DISALLOW_COPY_AND_ASSIGN(PetrinetOption);
};

#endif  // PETRINET_PETRINET_OPTION_H_
