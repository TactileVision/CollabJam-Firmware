#include "hardware_interfaces/iss_pcb_hw_interface.h"

IssPcbHardwareInterface::~IssPcbHardwareInterface(){};
IssPcbHardwareInterface::IssPcbHardwareInterface(uint8_t number_of_actuators){};

void IssPcbHardwareInterface::init(MultiplexedDrv* drvs,
                                   ActuatorConfig* actuator_conf,
                                   DrvConfig* drv_conf) {
  drvs_ = drvs;
  actuator_conf_ = actuator_conf;
  drv_conf_ = drv_conf;

  DEBUG_PRINTLN("[I2C-IF] Setting up interface");
  for (uint8_t i = 0; i < n_drvs_; i++) {
    deselectAllMultiplexer();
    drvs_[i].multiplexer_.select(i);
    drvs_[i].useAllRtpMode(drv_conf_->rtp_mode_unsigned);
  }
}

void IssPcbHardwareInterface::deselectAllMultiplexer() {
  for (uint8_t i = 0; i < 4; i++) {
    // TODO Double check addresses!
    drvs_[i].multiplexer_.deselectAll();
  }
}

void IssPcbHardwareInterface::setIntensity(uint8_t channel, float value) {
  //   uint16_t v = (uint16_t)(value * 256);
  //   if (v > 255) v = 255;
  //   tca_.select(channel_to_pin_map_[channel]);
  //   drvs_[channel].output((uint8_t)v);
}

void IssPcbHardwareInterface::setIntensity(uint8_t channel, uint8_t value) {
  // DRV2605L datasheet chapter 8.5.8.1.1 Open-Loop Mode
  DEBUG_PRINTLN(channel);
  if (channel > 32) {
    return;
  };
  /*
  0..7 - 0 0..7
  8..15 - 1 0..7
  16..23 - 2 0..7
  24..31 - 3 0..7
   */
  uint8_t mp = channel / 8;
  uint8_t c = channel - mp * 8;

  DEBUG_PRINTF("[ISS] Setting intensity on multiplexer %u channel %u\n", mp, c);
  deselectAllMultiplexer();
  drvs_[mp].multiplexer_.select(c);
  int x = map(value, 0, 254, 0, 127);
  drvs_[mp].drv_.drv_.setRealtimeValue(x);
}

void IssPcbHardwareInterface::setFrequency(uint8_t channel, uint32_t value) {
  /*
0..7 - 0 0..7
8..15 - 1 0..7
16..23 - 2 0..7
24..31 - 3 0..7
*/
  uint8_t mp = channel / 8;
  uint8_t c = channel - mp * 8;
  DEBUG_PRINTF("[ISS] Setting frequency on multiplexer %u channel %u\n", mp, c);
  actuator_conf_->frequency = value;
  deselectAllMultiplexer();
  drvs_[mp].multiplexer_.select(channel);
  drvs_[mp].drv_.setupActuator(*actuator_conf_);
  // drvs_->drv_.setupDRV(drv_config);
  // drvsmultiplexer_.select(channel);
  // drvsdrv_.drv_.setRealtimeValue(value);
}