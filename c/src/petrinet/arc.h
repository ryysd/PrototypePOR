#ifndef PETRINET_ARC_H_
#define PETRINET_ARC_H_

class PetrinetNode;
class Arc {
 public:
  Arc(const PetrinetNode* source, const PetrinetNode* target, int inscription)
    : source_(source), target_(target), inscription_(inscription) {}

  const PetrinetNode* source() const { return source_; }
  const PetrinetNode* target() const { return target_; }
  int inscription() const { return inscription_; }

 private:
  const PetrinetNode* source_;
  const PetrinetNode* target_;
  const int inscription_;

  DISALLOW_COPY_AND_ASSIGN(Arc);
};

#endif  // PETRINET_ARC_H_
