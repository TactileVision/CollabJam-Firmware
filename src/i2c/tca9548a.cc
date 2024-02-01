#include "i2c/tca9548a.h"
namespace tact {

TCA9548A::TCA9548A() {}

TCA9548A::~TCA9548A() {}

void TCA9548A::setup(uint8_t address) {
  address_ = address;
  read();
}

void TCA9548A::write(uint8_t reg) {
  Wire.beginTransmission(address_);
  Wire.write(reg);
  error_ = Wire.endTransmission();
#ifdef DEBUG_SERIAL_VERBOSE
  if (error_ == 0) {
    // printf("[TCA9548A][I2C][Success] %02hhX\n", reg);
    printf("[TCA9548A][I2C][Success] %02X\n", reg);
  } else {
    printf("[TCA9548A][I2C][Error] %02X\n", reg);
  }
#endif
}

/*
Select from channel 0 to 7*/
void TCA9548A::select(uint8_t channel) {
  if (channel > 7) return;

  state_ = 0x01 << channel;
  write(state_);
};

void TCA9548A::deselectAll() {
  write(0x00);
  state_ = 0x00;
};

uint8_t TCA9548A::read() {
  if (Wire.requestFrom(address_, (uint8_t)1) != 1) {
    return 0;
  }

  state_ = Wire.read();
  printf("%02hhX\n", state_);
  return state_;
};

uint8_t TCA9548A::getState() { return state_; }

}  // namespace tact