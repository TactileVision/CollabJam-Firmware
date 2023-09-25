#include "i2c/drv2605l.h"

DRV2605L::DRV2605L() {}
DRV2605L::~DRV2605L() {}

// Write a register and check write was successful
bool DRV2605L::drvWriteAndCheck(uint8_t reg, uint8_t value) {
  drv_.writeRegister8(reg, value);
  uint8_t read = drv_.readRegister8(reg);
  /* #ifdef DEBUG_TACTILE
    Serial.print("[DRV][I2C]:");
    Serial.print("[REG:");
    Serial.print(reg, HEX);
    Serial.print("] exp./read: ");
    Serial.print(value, HEX);
    Serial.print("/");
    Serial.print(read, HEX);
    Serial.println();
  #endif  // DEBUG_TACTILE */
  if (read != value) {
    // TODO Define and set errormsg;
    /* #ifdef DEBUG_TACTILE
        Serial.println(
            "[Error][DRV][I2C] Register state does not match specified value");
    #endif  // DEBUG_TACTILE */
    return false;
  }
  return true;
}

void DRV2605L::begin() { drv_.begin(); }

void DRV2605L::setupActuator(ActuatorConfig motor_config) {
  // setVoltagesClosedLoop(motor_config);
  setVoltagesOpenLoop(motor_config);
  if (motor_config.motor_type == MotorType::kLRA) {
    drv_.useLRA();
    // TODO: make lra open loop
    setLraFreqOpenLoop(motor_config.frequency);
  } else {
    drv_.useERM();
  }
}

void DRV2605L::setupDRV(DrvConfig drv_config) {
  drv_.selectLibrary(drv_config.pattern_library);  // datasheet p.38
  drv_.setMode(drv_config.mode);
  setLoopMode(drv_config.loop_mode);
}

void DRV2605L::setVoltagesOpenLoop(ActuatorConfig motor_config) {
  unsigned int clamp_voltage_reg = 0;
  if (motor_config.motor_type == MotorType::kLRA) {
    clamp_voltage_reg =
        motor_config.clamp_voltage /
        (21.32 * 1e-3 * sqrt(1 - motor_config.frequency * 800 * 1e-6));
  } else {
    clamp_voltage_reg = motor_config.clamp_voltage / (21.59 * 1e-3);
  }
  drv_.writeRegister8(0x17, clamp_voltage_reg);
}

void DRV2605L::setVoltagesClosedLoop(ActuatorConfig motor_config) {
  unsigned int rated_voltage_reg = 0;
  unsigned int clamp_voltage_reg = 0;
  // TODO: Differentiate between ERM and LRA. Current Implementation targets LRA
  if (motor_config.motor_type == MotorType::kLRA) {
    uint8_t control2_register = drv_.readRegister8(0x1C);
    uint8_t control5_register = drv_.readRegister8(0x1F);

    uint8_t sample_time_enum = (control2_register & 0b00110000) >> 4;
    uint8_t idiss_time_enum = (control2_register & 0b00000011) |
                              ((control5_register & 0b00000011) << 2);
    uint8_t blanking_time_enum = (control2_register & 0b000001100) >> 2 |
                                 ((control5_register & 0b00001100));

    uint8_t sample_time_us = kSamplingTimes[sample_time_enum];
    uint8_t blanking_time_us = kBlankingAndIdissTimes[blanking_time_enum];
    uint8_t idiss_time_us = kBlankingAndIdissTimes[idiss_time_enum];

    double drive_time_ms = 1000.f / (2.f * motor_config.frequency);

    // Only relevant in closed loop operation, in open loop OD_CLAMP ist used
    // for reference voltage
    rated_voltage_reg =
        motor_config.rated_voltage *
        sqrt(1 - (4 * (double)sample_time_us * 1e-6 + 300.f * 1e-6) *
                     motor_config.frequency) /
        (20.58 * 1e-3);

    // CLOSED Loop formula
    clamp_voltage_reg =
        motor_config.clamp_voltage *
        ((drive_time_ms * 1e-3) + (idiss_time_us * 1e-6) +
         (blanking_time_us * 1e-6)) /
        (21.64 * 1e-3 * ((drive_time_ms * 1e-3) - (300 * 1e-6)));
  } else {
    rated_voltage_reg = motor_config.rated_voltage / (21.18 * 1e-3);
    clamp_voltage_reg = motor_config.clamp_voltage / (21.22 * 1e-3);
  }
  drv_.writeRegister8(0x16, rated_voltage_reg);
  drv_.writeRegister8(0x17, clamp_voltage_reg);
}

void DRV2605L::setLraFreqOpenLoop(uint16_t freq) {
  unsigned int ol_lra_period = (uint8_t)((1000000 / freq) / 98.46);
  drv_.writeRegister8(0x20, ol_lra_period);
}

void DRV2605L::triggerLibOutput(unsigned int pattern_number) {
  if (pattern_number > 123) pattern_number = 123;

  drv_.setWaveform(0, pattern_number);  // play effect
  drv_.setWaveform(1, 0);               // end waveform
  drv_.go();
}

void DRV2605L::useRtpMode(bool unsigned_mode) {
  if (unsigned_mode) {
    // Set rtp register interpretation to
    // 100 scaling/ unsigned
    drv_.writeRegister8(DRV2605_REG_CONTROL3,
                        drv_.readRegister8(DRV2605_REG_CONTROL3) | 0b00001000);
  } else {
    drv_.writeRegister8(DRV2605_REG_CONTROL3,
                        drv_.readRegister8(DRV2605_REG_CONTROL3) & 0b11110111);
  }
  drv_.setMode(DRV2605_MODE_REALTIME);
}

void DRV2605L::setLoopMode(LoopMode mode) {
  switch (mode) {
    case LoopMode::kOpenLoop:
      useOpenLoopMode();
      break;
    case LoopMode::kClosedLoop:
      useClosedLoopMode();
      break;
  }
}

void DRV2605L::useOpenLoopMode() {
  // Set DRV2605_REG_CONTROL3[5] ERM, DRV2605_REG_CONTROL3[0] LRA to 1 (Open
  // Loop)
  drv_.writeRegister8(DRV2605_REG_CONTROL3,
                      drv_.readRegister8(DRV2605_REG_CONTROL3) | 0b00100001);
}
void DRV2605L::useClosedLoopMode() {
  // Set DRV2605_REG_CONTROL3[5] ERM, DRV2605_REG_CONTROL3[0] LRA to 0 (closed
  // Loop)
  drv_.writeRegister8(DRV2605_REG_CONTROL3,
                      drv_.readRegister8(DRV2605_REG_CONTROL3) & 0b11011110);
}
