#ifndef ESP32_VTPROTO_INTERFACE_H_
#define ESP32_VTPROTO_INTERFACE_H_

#include <Arduino.h>
#include <analogWrite.h>
#include <interface/hardware_interface.h>

uint8_t getPwmFromFloat(float value);
void setIntensity(uint8_t channel, float intensity);
void setFrequency(uint8_t channel, float intensity);

class EspVtprotoHardwareInterface : public tact::vtproto::HardwareInterface {
 public:
  EspVtprotoHardwareInterface(uint8_t number_of_actuators,
                              uint8_t* channel_to_pin_map);
  void setIntensity(uint8_t channel, float value);
  // virtual void setWaveform(uint8_t channel, waveform);
  void setFrequency(uint8_t channel, uint32_t value);
  int getActuatorCount();

 private:
  uint8_t number_of_actuators_;
  uint8_t* channel_pin_map_;
  uint8_t getPwmFromFloat(float value);
};
#endif  // !ESP32_VTPROTO_INTERFACE_H_