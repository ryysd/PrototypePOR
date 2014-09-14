#ifndef PETRINET_TRANSITION_H_
#define PETRINET_TRANSITION_H_

#include <string>

class Transition : public PetrinetNode {
 public:
  Transition(const std::string& id, const std::string& name)
    : PetrinetNode(id, name) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(Transition);
};

#endif  // PETRINET_TRANSITION_H_
