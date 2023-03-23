#ifndef TACTON_H_
#define TACTON_H_
#include <Arduino.h>
#include <vtproto.h>

namespace tact {
namespace vtproto {

#ifndef VTPROTO_TACTON_MAX_INSTRUCTIONS
#define VTPROTO_TACTON_MAX_INSTRUCTIONS 512
#endif

#ifndef VTPROTO_HEADER_MAX_STRLEN
#define VTPROTO_HEADER_MAX_STRLEN 32
#endif
typedef struct TactonHeader {
  FileHeader file_header_;
  char author_[VTPROTO_HEADER_MAX_STRLEN],
      pattern_name_[VTPROTO_HEADER_MAX_STRLEN];

 public:
  TactonHeader();
} TactonHeader;

typedef struct Tacton {
  TactonHeader header_;
  Instruction instructions_[VTPROTO_TACTON_MAX_INSTRUCTIONS];
  GroupManager group_manager_;
  uint8_t length_;
#ifdef DEBUG_TACTILE
  void print();
#endif
} Tacton;
}  // namespace vtproto

}  // namespace tact
#endif  // !TACTON_H_