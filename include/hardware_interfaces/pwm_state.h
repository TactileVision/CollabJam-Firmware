#ifndef TV_PWM_STATE_H_
#define TV_PWM_STATE_H_

#include <Arduino.h>

#include "config.h"

namespace tact {
namespace vtproto {

class PwmState {
 private:
  float pins_[TACTILE_DISPLAY_NUM_OUTPUTS];

 public:
  PwmState();
  void setPinValue(uint8_t pin, float value);
  uint8_t getNumberOfActivePins();
  bool stateWillChange(uint8_t pin, float value);
  float getPinValue(uint8_t pin);
  void printState();
  ~PwmState();
};

#ifndef COMPENSATION_OFFSET
#define COMPENSATION_OFFSET 0.25
#endif

#ifndef COMPENSATION_NEW_MIN
#define COMPENSATION_NEW_MIN 0.3
#endif

#ifndef COMPENSATION_NEW_MAX
#define COMPENSATION_NEW_MAX 1.0 - COMPENSATION_OFFSET
#endif

float getRerangedFloat(float value, float new_min, float new_max);
float getCompensatedFloat(float value, float uncompensated_min,
                          float uncompensated_max, float compensation_range,
                          uint8_t num_of_motors);

}  // namespace vtproto

}  // namespace tact
#endif  // TV_PWM_STATE_H_