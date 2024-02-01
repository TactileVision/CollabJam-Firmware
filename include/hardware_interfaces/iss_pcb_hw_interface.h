// TODO: Integrate newly created drv lib from newer projects
//  #ifndef I2C_MP_DRV_HW_INTERFACE_H_
//  #define I2C_MP_DRV_HW_INTERFACE_H_

// #include "Arduino.h"
// #include "drv2605l.h"

// // i2c-components-lib
// #include "tca9548a.h"
#ifndef ISS_PCB_HW_INTERFACE_H_
#define ISS_PCB_HW_INTERFACE_H_

#include <hardware_interfaces/hardware_interface.h>

#include "Arduino.h"
#include "debug_util.h"
#include "hardware/multiplexed_drv.h"

class IssPcbHardwareInterface : public HardwareInterface {
 public:
  IssPcbHardwareInterface(uint8_t number_of_actuators);
  ~IssPcbHardwareInterface();
  void setIntensity(uint8_t channel, float value);
  void setIntensity(uint8_t channel, uint8_t value);
  void setFrequency(uint8_t channel, uint32_t value);
  int getActuatorCount() { return num_of_actuators_; };
  void init(MultiplexedDrv* drvs_, ActuatorConfig* actuator_conf_,
            DrvConfig* drv_conf_);

 private:
  MultiplexedDrv* drvs_;
  uint8_t n_drvs_ = 4;
  ActuatorConfig* actuator_conf_;
  DrvConfig* drv_conf_;
  int num_of_actuators_;
  uint8_t* channel_to_pin_map_;
  void deselectAllMultiplexer();
  // MultiplexedDrv drvs;
};


#endif  // !I2C_MP_DRV_INTERFACE_H_