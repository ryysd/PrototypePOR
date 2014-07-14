#ifndef TRANSITION_SYSTEM_ACTION_TABLE_H_
#define TRANSITION_SYSTEM_ACTION_TABLE_H_

#include <map>
#include <string>
#include <vector>

typedef std::vector<std::string> EntitySet;
class ActionTable {
 public:
  ActionTable() {}
  ~ActionTable() { FreeActions(); }

  Action* Register(Action* action) { return actions_.insert(make_pair(action->name(), action)).first->second; }

  Action* Create(const std::string& name, const EntitySet& creator, const EntitySet& eraser, const EntitySet& reader, const EntitySet& embargoes) {
    Action* old = FindByName(name);
    return old == NULL ? Register(new Action(name, creator, eraser, reader, embargoes)) : old;
  }

  Action* FindByName(const std::string& name) const {
    auto it = actions_.find(name);
    return (it != actions_.end()) ? it->second : NULL;
  }

  const std::map<std::string, Action*> actions() const { return actions_; }

 private:
  void FreeActions() { for (auto &kv : actions_) { delete kv.second; } }

  std::map<std::string, Action*> actions_;

  DISALLOW_COPY_AND_ASSIGN(ActionTable);
};

#endif  // TRANSITION_SYSTEM_ACTION_TABLE_H_