#ifndef TACTON_STORE_H_
#define TACTON_STORE_H_

#include <Arduino.h>

#include "tacton/tacton.h"

namespace tact {
namespace vtproto {

#ifndef TACTON_STORE_SIZE
#define TACTON_STORE_SIZE 3
#endif

class TactonStore {
 private:
  Tacton tactons_[TACTON_STORE_SIZE];
  // uint8_t head_, tail_;
  uint8_t last_new_, count_;

 public:
  TactonStore();
  Tacton* getNewTacton();
  Tacton* getLastTacton();
  // Tacton* getTactonAt(uint8_t index);
  void reset();
  bool isFull();
  uint8_t getCurrentLength();
  //   uint8_t getMaxLength();
};

}  // namespace vtproto

}  // namespace tact

#endif  // !TACTON_STORE_H_