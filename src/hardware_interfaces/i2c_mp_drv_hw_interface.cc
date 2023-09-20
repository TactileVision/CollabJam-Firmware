#include "hardware_interfaces/i2c_mp_drv_hw_interface.h"

namespace tact {
MultiplexedDrvsInterface::~MultiplexedDrvsInterface(){};
MultiplexedDrvsInterface::MultiplexedDrvsInterface(
    uint8_t number_of_actuators, uint8_t* channel_to_pin_map){};

// void MultiplexedDrvsInterface::init() {
//   tact::ActuatorConfig actuator_conf = {tact::MotorType::kLRA, 1.0, 1.0,
//   170}; tact::DrvConfig drv_conf = {DRV2605_MODE_REALTIME, 2,
//                               tact::LoopMode::kOpenLoop, false};

//   Serial.println("Foo");
//   DEBUG_PRINTLN("[I2C-IF] Settign up interface");
//   drvs.initAllDrvs(actuator_conf, drv_conf);
//   drvs.pingAllOutputs();
// }
void MultiplexedDrvsInterface::init(MultiplexedDrv* drvs,
                                    ActuatorConfig* actuator_conf,
                                    DrvConfig* drv_conf) {
  drvs_ = drvs;
  actuator_conf_ = actuator_conf;
  drv_conf_ = drv_conf;

  DEBUG_PRINTLN("[I2C-IF] Setting up interface");
  drvs_->useAllRtpMode(drv_conf_->rtp_mode_unsigned);
  // drvs_->initAllDrvs(*actuator_conf, *drv_conf);
  // drvs_->pingAllOutputs();
}
void MultiplexedDrvsInterface::setIntensity(uint8_t channel, float value) {
  //   uint16_t v = (uint16_t)(value * 256);
  //   if (v > 255) v = 255;
  //   tca_.select(channel_to_pin_map_[channel]);
  //   drvs_[channel].output((uint8_t)v);
}

void MultiplexedDrvsInterface::setIntensity(uint8_t channel, uint8_t value) {
  //DRV2605L datasheet chapter 8.5.8.1.1 Open-Loop Mode 
  drvs_->multiplexer_.select(channel);
  int x = map(value, 0, 254, 0, 127);
  drvs_->drv_.drv_.setRealtimeValue(x);
}
void MultiplexedDrvsInterface::setFrequency(uint8_t channel, uint32_t value) {
  // drvsmultiplexer_.select(channel);
  // drvsdrv_.drv_.setRealtimeValue(value);
}

}  // namespace tact
