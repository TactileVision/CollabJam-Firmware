#include "hardware_interfaces/drvs_hw_interface.h"

MultiplexedDrvsInterface::~MultiplexedDrvsInterface(){};
MultiplexedDrvsInterface::MultiplexedDrvsInterface(
    uint8_t number_of_actuators){};

void MultiplexedDrvsInterface::init(MultiplexedDrv* drvs,
                                    ActuatorConfig* actuator_conf,
                                    DrvConfig* drv_conf) {
  drvs_ = drvs;
  actuator_conf_ = actuator_conf;
  drv_conf_ = drv_conf;

  DEBUG_PRINTLN("[I2C-IF] Setting up interface");
  drvs_->useAllRtpMode(drv_conf_->rtp_mode_unsigned);
}
void MultiplexedDrvsInterface::setIntensity(uint8_t channel, float value) {
  //   uint16_t v = (uint16_t)(value * 256);
  //   if (v > 255) v = 255;
  //   tca_.select(channel_to_pin_map_[channel]);
  //   drvs_[channel].output((uint8_t)v);
}

void MultiplexedDrvsInterface::setIntensity(uint8_t channel, uint8_t value) {
  // DRV2605L datasheet chapter 8.5.8.1.1 Open-Loop Mode
  DEBUG_PRINTLN("Setting intensity");
  drvs_->multiplexer_.select(channel);
  int x = map(value, 0, 254, 0, 127);
  drvs_->drv_.drv_.setRealtimeValue(x);
}

void MultiplexedDrvsInterface::setFrequency(uint8_t channel, uint32_t value) {
  actuator_conf_->frequency = value;
  drvs_->multiplexer_.select(channel);
  drvs_->drv_.setLraFreqOpenLoop(actuator_conf_->frequency);
}