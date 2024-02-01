#ifndef HARDWARE_INTERFACE_H_
#define HARDWARE_INTERFACE_H_

#include <Arduino.h>

// each of the 32 bits represents a flag for each output. if bit is high, it
// means that output can change amplitude or frequency
struct TactileDisplayInformation {
  uint32_t num_outputs;
  uint32_t can_change_amplitude;
  uint32_t can_change_frequency;
};

struct TactileDisplayFrequencyInformation {
  uint16_t f_min;
  uint16_t f_max;
  uint16_t f_resonance;
};

class HardwareInterface {
 public:
  virtual void setIntensity(uint8_t channel, float value);
  virtual void setIntensity(uint8_t channel, uint8_t value);
  virtual void setFrequency(uint8_t channel, uint32_t value);
  virtual int getActuatorCount();
};

#endif  // !HARDWARE_INTERFACE_H_