#ifndef HARDWARE_INTERFACE_H_
#define HARDWARE_INTERFACE_H_

#include <Arduino.h>

namespace tact {
namespace vtproto {
class HardwareInterface {
 public:
  virtual void setIntensity(uint8_t channel, float value);
  // virtual void setWaveform(uint8_t channel, waveform);
  virtual void setFrequency(uint8_t channel, uint32_t value);
  virtual int getActuatorCount();
};
}  // namespace vtproto
}  // namespace tact

#endif  // !HARDWARE_INTERFACE_H_