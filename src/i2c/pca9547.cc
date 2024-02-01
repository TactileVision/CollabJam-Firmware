#include "i2c/pca9547.h"
namespace tact {

PCA9547::PCA9547() {}

PCA9547::~PCA9547() {}

void PCA9547::setup(uint8_t address) {
  address_ = address;
  read();
}

void PCA9547::write(uint8_t reg) {
  Wire.beginTransmission(address_);
  Wire.write(reg);
  error_ = Wire.endTransmission();
#ifdef DEBUG_SERIAL_VERBOSE
  if (error_ == 0) {
    // printf("[PCA9547][I2C][Success] %02hhX\n", reg);
    printf("[PCA9547][I2C][Success] %02X\n", reg);
  } else {
    printf("[PCA9547][I2C][Error] %02X\n", reg);
  }
#endif
}

/*
Select from channel 0 to 7*/
void PCA9547::select(uint8_t channel) {
  if (channel > 7) return;

  state_ = (channel & 0b00000111) | 0b00001000;
  write(state_);
};

void PCA9547::deselectAll() {
  write(0x00);
  state_ = 0x00;
};

uint8_t PCA9547::read() {
  if (Wire.requestFrom(address_, (uint8_t)1) != 1) {
    return 0;
  }

  state_ = Wire.read();
  printf("%02hhX\n", state_);
  return state_;
};

}  // namespace tact