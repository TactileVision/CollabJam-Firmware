#ifndef COLLABJAMV2_HW_INTERFACE_H_
#define COLLABJAMV2_HW_INTERFACE_H_
#include <hardware_interfaces/hardware_interface.h>

#include "Arduino.h"
#include "Haptic_Driver.h"
#include "debug_util.h"

class CollabJamV2HardwareInterface : public HardwareInterface {
 public:
  CollabJamV2HardwareInterface();
  ~CollabJamV2HardwareInterface();
  void setIntensity(uint8_t channel, float value);
  void setIntensity(uint8_t channel, uint8_t value);
  void setFrequency(uint8_t channel, uint32_t value);
  int getActuatorCount() { return 4; };
  void init();

 private:
  Haptic_Driver actor_[4];
};
#endif