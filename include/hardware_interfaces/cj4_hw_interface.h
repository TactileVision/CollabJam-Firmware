#ifndef COLLABJAMV2_HW_INTERFACE_H_
#define COLLABJAMV2_HW_INTERFACE_H_
#include <hardware_interfaces/hardware_interface.h>
#include "i2c/tca9548a.h"
#include "Arduino.h"
#include "Haptic_Driver.h"
#include "debug_util.h"

class CollabJamV4HardwareInterface : public HardwareInterface {
 public:
  CollabJamV4HardwareInterface();
  ~CollabJamV4HardwareInterface();
  void setIntensity(uint8_t channel, float value);
  void setIntensity(uint8_t channel, uint8_t value);
  void setFrequency(uint8_t channel, uint32_t value);
  int getActuatorCount() { return 6; };
  void init();

 private:
  
  Haptic_Driver actor_[6];
  tact::TCA9548A mp_;
};
#endif