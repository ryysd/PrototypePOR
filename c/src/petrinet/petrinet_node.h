#ifndef PETRINET_PETRINET_NODE_H_
#define PETRINET_PETRINET_NODE_H_

#include <vector>
#include <string>

class PetrinetNode {
 public:
  PetrinetNode(const std::string& id, const std::string& name)
    : id_(id), name_(name) {}

  void AddArc(Arc* arc) const { arcs_.push_back(std::unique_ptr<Arc>(arc)); }
  const Arc* FindArc(const PetrinetNode* node) const {
    auto result = std::find_if(arcs_.begin(), arcs_.end(), [node](const std::unique_ptr<Arc>& arc) { return arc->target()->Equals(*node); });
    return (result != arcs_.end()) ? result->get() : nullptr;
  }

  bool Equals(const PetrinetNode& other) const { return id_ == other.id(); }

  const std::string& id() const { return id_; }
  const std::string& name() const { return name_; }
  const std::vector<std::unique_ptr<Arc>>& arcs() const { return arcs_; }
 private:
  const std::string id_;
  const std::string name_;
  mutable std::vector<std::unique_ptr<Arc>> arcs_;

  DISALLOW_COPY_AND_ASSIGN(PetrinetNode);
};

#endif  // PETRINET_PETRINET_NODE_H_
