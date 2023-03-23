#ifndef GROUPS_H_
#define GROUPS_H_

#include <Arduino.h>
#include <Array.h>
#include <Set.h>

namespace tact {
namespace vtproto {

typedef struct Group {
  uint32_t id_;
  Set channels_, groups_;
  Group() : id_(0){};
#ifdef DEBUG_TACTILE
  void print();
#endif
  void reset();
} Group;

#ifndef VTPROTO_MAX_GROUPS
#define VTPROTO_MAX_GROUPS 32
#endif
class GroupManager {
 private:
  int16_t getArrayIndexByGroupId(uint32_t group_id);
  Array<tact::vtproto::Group, VTPROTO_MAX_GROUPS> groups_;

 public:
  bool addToGroupList(bool add_groups, tact::vtproto::Group& group);
  Group* getGroupByGroupId(uint8_t group_id);
  uint8_t getLength() { return groups_.size(); };
  void reset();
#ifdef DEBUG_TACTILE
  void print();
#endif
};
}  // namespace vtproto

}  // namespace tact

#endif