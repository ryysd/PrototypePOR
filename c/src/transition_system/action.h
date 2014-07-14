#ifndef TRANSITION_SYSTEM_ACTION_H_
#define TRANSITION_SYSTEM_ACTION_H_

#include <map>
#include <string>
#include <vector>

/*! @class Action
 *  @brief class representing action
*/
class Action {
 public:
  typedef std::vector<std::string> EntitySet;
  Action(const std::string name, EntitySet creator, EntitySet eraser, EntitySet reader, EntitySet embargoes)
    : name_(name), creator_(creator), reader_(reader), eraser_(eraser), embargoes_(embargoes) {}

  void Simulate(const Action* action) { simulates_.insert(make_pair(action->name(), true)); }
  void Disable(const Action* action) { disables_.insert(make_pair(action->name(), true)); }

  bool isSimulate(const Action* action) const { return simulates_.find(action->name()) != simulates_.end(); }
  bool isDisable(const Action* action) const { return disables_.find(action->name()) != disables_.end(); }

  const std::string& name() const { return name_; }
  const EntitySet& creator() const { return creator_; }
  const EntitySet& reader() const { return reader_; }
  const EntitySet& eraser() const { return eraser_; }
  const EntitySet& embargoes() const { return embargoes_; }

 private:
  const std::string name_;
  const EntitySet creator_;
  const EntitySet reader_;
  const EntitySet eraser_;
  const EntitySet embargoes_;

  std::map<std::string, bool> simulates_;
  std::map<std::string, bool> disables_;

  DISALLOW_COPY_AND_ASSIGN(Action);
};

#endif  // TRANSITION_SYSTEM_ACTION_H_
