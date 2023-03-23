#include "tacton/tacton.h"

namespace tact {
namespace vtproto {

TactonHeader::TactonHeader() {
  author_[0] = VTPROTO_HEADER_MAX_STRLEN;
  pattern_name_[0] = VTPROTO_HEADER_MAX_STRLEN;
}

#ifdef DEBUG_TACTILE
void Tacton::print() {
  Serial.print("Author: ");
  Serial.println(&this->header_.author_[2]);
  Serial.print("Pattern: ");
  Serial.println(&this->header_.pattern_name_[2]);
  Serial.print("Duration: ");
  Serial.println(this->header_.file_header_.duration_ms);
  Serial.print("Instructions: ");
  Serial.println(this->header_.file_header_.n_instructions);
  Serial.print("Channels: ");
  Serial.println(this->header_.file_header_.n_channels);
  Serial.println();

  for (uint8_t i = 0; i < this->header_.file_header_.n_instructions; i++) {
    Serial.print("Instruction ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(this->instructions_[i].which_concrete_instruction);
  }
  // this->group_manager_.print();
}
#endif  // DEBUG_TACTILE
}  // namespace vtproto

}  // namespace tact
