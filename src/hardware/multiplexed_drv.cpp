#include "hardware/multiplexed_drv.h"

const unsigned int MultiplexedDrv::kNumberOfActuators = 5;
MultiplexedDrv::MultiplexedDrv(/* args */) {}

MultiplexedDrv::~MultiplexedDrv() {}

void MultiplexedDrv::setMultiplexerAddress(uint8_t addr) { mp_addr_ = addr; }

void MultiplexedDrv::initAllDrvs(bool* use_lra_per_channel, uint8_t len) {
  multiplexer_.setup(mp_addr_);
  for (int i = 0; i < len; i++) {
    DEBUG_PRINTF("SETTING UP DRV %u\n", i);
    multiplexer_.select(i);
    drv_.begin();
    use_lra_per_channel[i] ? drv_.drv_.useLRA() : drv_.drv_.useERM();
    // use_lra_per_channel[i] ? drv_.selectLibrary(6)
    //                        : drv_.selectLibrary(2);  // datasheet p.38
    drv_.drv_.selectLibrary(2);  // datasheet p.38
    if (use_lra_per_channel[i])
      // setupLRA(2.5, 2.5,
      //          170);  // clamp voltage is rated voltage in open loop mode
      drv_.drv_.setMode(DRV2605_MODE_INTTRIG);
    delay(250);
  }
}

void MultiplexedDrv::initAllDrvs(ActuatorConfig actutator_config,
                                 DrvConfig drv_config) {
  multiplexer_.setup(mp_addr_);
  for (int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.begin();
    drv_.setupActuator(actutator_config);
    drv_.setupDRV(drv_config);
    DEBUG_PRINTF("[DRV] Initialized Driver %i\n", i);
    delay(250);
  }
}

void MultiplexedDrv::updateAllSettings(ActuatorConfig actutator_config,
                                       DrvConfig drv_config) {
  for (int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.setupActuator(actutator_config);
    drv_.setupDRV(drv_config);
    delay(25);
  }
}

void MultiplexedDrv::initAllDrvs(bool use_lra) {
  bool x[8] = {use_lra, use_lra, use_lra, use_lra,
               use_lra, use_lra, use_lra, use_lra};
  initAllDrvs(x, 8);
}

void MultiplexedDrv::pingAllOutputs() {
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.triggerLibOutput(1);
    delay(250);
  }
}

void MultiplexedDrv::setAllGoBitsLow() {
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.drv_.stop();
  }
}

void MultiplexedDrv::stopAllOutputs() {
  stopRtpOutputs();
  stopLibOutputs();
}

void MultiplexedDrv::stopRtpOutputs() {
  // Check if unsigned_mode is active or not by reading register 0x1D[3]
  uint8_t reg = drv_.drv_.readRegister8(DRV2605_REG_CONTROL3);
  useAllRtpMode((reg & 0b00001000) > 0);
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.drv_.setRealtimeValue(0);
  }
}

void MultiplexedDrv::stopLibOutputs() {
  useAllLibMode();
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.drv_.setWaveform(0, 0);  // play effect
    drv_.drv_.go();
  }
}

void MultiplexedDrv::useAllRtpMode(bool unsigned_mode) {
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.useRtpMode(unsigned_mode);
    delay(10);
  }
}

void MultiplexedDrv::useAllLibMode() {
  for (unsigned int i = 0; i < MultiplexedDrv::kNumberOfActuators; i++) {
    multiplexer_.select(i);
    drv_.drv_.setMode(DRV2605_MODE_INTTRIG);
  }
}
