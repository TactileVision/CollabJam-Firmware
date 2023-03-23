// #include "hardware_interfaces/i2c_mp_drv_hw_interface.h"

// namespace tact {
// I2CMultiplexerDrvInterface::~I2CMultiplexerDrvInterface(){};
// I2CMultiplexerDrvInterface::I2CMultiplexerDrvInterface(
//     uint8_t number_of_actuators, uint8_t* channel_to_pin_map)
//     : num_of_actuators_(number_of_actuators),
//       channel_to_pin_map_(channel_to_pin_map) {}
// void I2CMultiplexerDrvInterface::init() {
//   while (!Wire.begin())
//     ;
//   tca_.setup(0x70);
//   for (uint8_t i = 0; i < num_of_actuators_; i++) {
//     tca_.select(channel_to_pin_map_[i]);
//     drvs_[i].setup(DEFAULT_DRV_MOTOR_CONFIG, DEFAULT_DRV_CHANNEL_CONFIG);
//     drvs_[i].useRtpMode(true);
//     drvs_[i].output((uint8_t)255);
//     delay(250);
//     drvs_[i].output((uint8_t)0);
//   }
// }

// void I2CMultiplexerDrvInterface::setIntensity(uint8_t channel, float value) {
//   uint16_t v = (uint16_t)(value * 256);
//   if (v > 255) v = 255;
//   tca_.select(channel_to_pin_map_[channel]);
//   drvs_[channel].output((uint8_t)v);
// }
// void I2CMultiplexerDrvInterface::setFrequency(uint8_t channel, uint32_t value) {
// }

// }  // namespace tact
