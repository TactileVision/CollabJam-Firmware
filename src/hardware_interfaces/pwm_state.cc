
#include "hardware_interfaces/pwm_state.h"

namespace tact {
namespace vtproto {

PwmState::PwmState() {}

PwmState::~PwmState() {}
void PwmState::setPinValue(uint8_t pin, float value) {
  if (pin >= TACTILE_DISPLAY_NUM_OUTPUTS) {
    return;
  }
  pins_[pin] = value;
}

uint8_t PwmState::getNumberOfActivePins() {
  uint8_t x = 0;
  for (uint8_t i = 0; i < TACTILE_DISPLAY_NUM_OUTPUTS; i++) {
    if (pins_[i] > 0) ++x;
  }
  return x;
}

void PwmState::printState() {
  Serial.println("PWM pin state:");
  for (uint8_t i = 0; i < TACTILE_DISPLAY_NUM_OUTPUTS; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(pins_[i]);
    /* code */
  }
}

bool PwmState::stateWillChange(uint8_t pin, float value) {
  if (pin >= TACTILE_DISPLAY_NUM_OUTPUTS) return false;
  return (value == 0 && pins_[pin] > 0) || (value != 0 && pins_[pin] == 0);
}

float PwmState::getPinValue(uint8_t pin) {
  if (pin >= TACTILE_DISPLAY_NUM_OUTPUTS) return 0;
  return pins_[pin];
}

float getRerangedFloat(float value, float new_min, float new_max) {
  return (new_max - new_min) * value + new_min;
}
float getCompensatedFloat(float value, float uncompensated_min,
                          float uncompensated_max, float compensation_range,
                          uint8_t num_of_motors) {
  float x = getRerangedFloat(value, uncompensated_min, uncompensated_max);
  // Serial.print("Reranged ");
  // Serial.println(x);
  float y =
      x + (compensation_range / TACTILE_DISPLAY_NUM_OUTPUTS) * num_of_motors;
  return y;
  // return getRerangedFloat(value, uncompensated_min, uncompensated_max) +
  //        (compensation_range * (1 / num_of_motors));
}

}  // namespace vtproto

}  // namespace tact