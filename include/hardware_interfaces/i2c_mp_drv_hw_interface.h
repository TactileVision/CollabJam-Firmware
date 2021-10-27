#ifndef I2C_MP_DRV_HW_INTERFACE_H_
#define I2C_MP_DRV_HW_INTERFACE_H_

#include <interface/hardware_interface.h>

#include "Arduino.h"
#include "config.h"
#include "drv2605l.h"

// i2c-components-lib
#include "tca9548a.h"

namespace tact {

class I2CMultiplexerDrvInterface : public tact::vtproto::HardwareInterface {
 public:
  I2CMultiplexerDrvInterface(uint8_t number_of_actuators,
                             uint8_t* channel_to_pin_map);
  ~I2CMultiplexerDrvInterface();
  void setIntensity(uint8_t channel, float value);
  void setFrequency(uint8_t channel, uint32_t value);
  int getActuatorCount() { return num_of_actuators_; };
  void init();

 private:
  int num_of_actuators_;
  uint8_t* channel_to_pin_map_;
  tact::TCA9548A tca_;
  tact::DRV2605L drvs_[config::display::kNumOfOutputs];
  // Adafruit_DRV2605 drv_;
};

}  // namespace tact

#endif  // !I2C_MP_DRV_INTERFACE_H_