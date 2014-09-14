#ifndef PETRINET_PLACE_H_
#define PETRINET_PLACE_H_

#include <string>

class Place : public PetrinetNode {
 public:
  Place(const std::string& id, const std::string& name, int initial_marking)
  : PetrinetNode(id, name), initial_marking_(initial_marking) {}

  int initial_marking() const { return initial_marking_; }
 private:
  const int initial_marking_;

  DISALLOW_COPY_AND_ASSIGN(Place);
};

#endif  // PETRINET_PLACE_H_
