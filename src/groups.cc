#include "tacton/groups.h"
namespace tact {
namespace vtproto {

int16_t GroupManager::getArrayIndexByGroupId(uint32_t group_id) {
  // groups_.size()
  for (uint i = 0; i < groups_.size(); i++) {
    if (groups_.at(i).id_ == group_id) return i;
  }
  return -1;
}

bool GroupManager::addToGroupList(bool add_groups,
                                  tact::vtproto::Group& group) {
  if (group.id_ == 0) {
#ifdef DEBUG_TACTILE
    Serial.println("Group id 0 is reserved, cannot add specified group");
#endif  // DEBUG_TACTILE
    return false;
  }

  int index = getArrayIndexByGroupId(group.id_);

  if (index == -1)  // group id not found, add to Array
  {
    if (groups_.full()) return false;

    // groups_.push_back(Group{.id_ = group.id_});
    Group g;
    g.id_ = group.id_;
    groups_.push_back(g);
    index = getArrayIndexByGroupId(group.id_);

    if (index == -1) return false;
  }

  if (add_groups)
    // groups_.at(index).groups_ = groups_.at(index).groups_ + group.groups_;
    groups_.at(index).groups_ += group.groups_;
  else
    groups_.at(index).channels_ += group.channels_;
  // groups_.at(index).channels_ = groups_.at(index).channels_ +
  // group.channels_;
  return true;
}

Group* GroupManager::getGroupByGroupId(uint8_t group_id) {
  int index = getArrayIndexByGroupId(group_id);
  if (index == -1) return NULL;

  return &groups_.at(index);
}

void Group::reset() {
  this->id_ = 0;
  this->channels_.clear();
  this->groups_.clear();
}
void GroupManager::reset() {
  for (uint8_t i = 0; i < this->groups_.size(); i++) {
    this->groups_.at(i).reset();
  }
  this->groups_.clear();
}
#ifdef DEBUG_TACTILE
void Group::print() {
  Serial.print("Group ");
  Serial.print(this->id_);
  Serial.print(" with ");
  Serial.print(this->channels_.count());
  Serial.print(" Channels(");
  for (uint8_t i = 0; i < this->channels_.count(); i++) {
    Serial.print(this->channels_.getNth(i + 1));
    Serial.print(" ");
  }
  Serial.println(")");
}
void GroupManager::print() {
  Serial.print(this->groups_.size());
  Serial.println(" Groups");
  for (uint8_t i = 0; i < this->groups_.size(); i++) {
    this->groups_.at(i).print();
  }
}
#endif  // DEBUG_TACTILE

}  // namespace vtproto

}  // namespace tact