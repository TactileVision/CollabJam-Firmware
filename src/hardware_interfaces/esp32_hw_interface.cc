#include "hardware_interfaces/esp32_hw_interface.h"

EspVtprotoHardwareInterface::EspVtprotoHardwareInterface(
    uint8_t number_of_actuators, uint8_t* channel_to_pin_map) {
  this->number_of_actuators_ = number_of_actuators;
  this->channel_pin_map_ = channel_to_pin_map;
  for (uint8_t i = 0; i < this->number_of_actuators_; i++) {
    pinMode(this->channel_pin_map_[i], OUTPUT);
  }
};

uint8_t EspVtprotoHardwareInterface::getPwmFromFloat(float value) {
  uint16_t v = (uint16_t)(value * 256);
  return v < 255 ? v : 255;
}

void EspVtprotoHardwareInterface::setIntensity(uint8_t channel, float value) {
  if (channel > this->number_of_actuators_) {
#ifndef DEBUG
    Serial.println("Channel number to high, does not exsist");
#endif  // !DEBUG
  } else {
#ifdef DEBUG
    Serial.print("Setting value to intensity ");
    // Serial.printvalue);
    // Serial.print("/");
    // Serial.println(getPwmFromFloat(value));
#endif  // !DEBUG
    analogWrite(this->channel_pin_map_[channel], this->getPwmFromFloat(value));
#ifdef DEBUG
    Serial.println("setIntensity done");
#endif  // !DEBUG
  }
};
void EspVtprotoHardwareInterface::setIntensity(uint8_t channel, uint8_t value) {
  if (channel > this->number_of_actuators_) {
#ifndef DEBUG
    Serial.println("Channel number to high, does not exsist");
#endif  // !DEBUG
  } else {
#ifdef DEBUG
    Serial.print("Setting value to intensity ");
    // Serial.printvalue);
    // Serial.print("/");
    // Serial.println(getPwmFromFloat(value));
#endif  // !DEBUG
    analogWrite(this->channel_pin_map_[channel], value);
#ifdef DEBUG
    Serial.println("setIntensity done");
#endif  // !DEBUG
  }
};

void EspVtprotoHardwareInterface::setFrequency(uint8_t channel, uint32_t value){

};

int EspVtprotoHardwareInterface::getActuatorCount() {
  return this->number_of_actuators_;
};