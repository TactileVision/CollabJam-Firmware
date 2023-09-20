#ifndef HARDWARE_INTERFACE_H_
#define HARDWARE_INTERFACE_H_

#include <Arduino.h>

namespace tact {

// each of the 32 bits represents a flag for each output. if bit is high, it
// means that output can change amplitude or frequency
struct TactileDisplayInformation {
  uint32_t num_outputs;
  uint32_t can_change_amplitude;
  uint32_t can_change_frequency;
};
namespace vtproto {
class HardwareInterface {
 public:
  virtual void setIntensity(uint8_t channel, float value);
  virtual void setIntensity(uint8_t channel, uint8_t value);
  virtual void setFrequency(uint8_t channel, uint32_t value);
  virtual int getActuatorCount();
};
}  // namespace vtproto
}  // namespace tact

#endif  // !HARDWARE_INTERFACE_H_