#include "tacton/tacton_store.h"

namespace tact {
namespace vtproto {

TactonStore::TactonStore() {
  this->last_new_ = TACTON_STORE_SIZE - 1;
  // this->head_ = 0;
  // this->tail_ = 0;
  this->count_ = 0;
}

// void TactonStore::storeTacton(Tacton tacton) {
//   this->tactons_[0] = tacton;
//   this->count_ = 1;
// }
Tacton* TactonStore::getNewTacton() {
  // #ifdef DEBUG_TACTILE
  // Serial.print("Before: ");
  // Serial.println(this->last_new_);
  // #endif // DEBUG_TACTILE

  this->last_new_ = (this->last_new_ + 1) % TACTON_STORE_SIZE;
  this->tactons_[this->last_new_].group_manager_.reset();
  //  this->tactons_[this->last_new_] = Tacton();

  // #ifdef DEBUG_TACTILE
  // Serial.print("After: ");
  // Serial.println(this->last_new_);
  // #endif // DEBUG_TACTILE

  return &this->tactons_[this->last_new_];
}
Tacton* TactonStore::getLastTacton() {
  return &this->tactons_[this->last_new_];
}
// Tacton* TactonStoregetTactonAt(uint8_t index);
void TactonStore::reset() { this->count_ = 0; }
bool TactonStore::isFull() { return this->count_ == TACTON_STORE_SIZE; }
uint8_t TactonStore::getCurrentLength() { return this->count_; }

}  // namespace vtproto
}  // namespace tact