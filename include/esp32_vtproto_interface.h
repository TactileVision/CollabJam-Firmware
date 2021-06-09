#ifndef ESP32_VTPROTO_INTERFACE_H_
#define ESP32_VTPROTO_INTERFACE_H_

#include <Arduino.h>
#include <analogWrite.h>

const uint8_t kNumOfOutputs = 9;
const uint8_t kChannelToPinMap[kNumOfOutputs] = {
    12, //ISS Helmet Front, clockwise
    15,
    2,
    4,
    26,
    25,
    33,
    13,
    27 // TOP
};
uint8_t getPwmFromFloat(float value);
void setIntensity(uint8_t channel, float intensity);
void setFrequency(uint8_t channel, float intensity);

#endif // !ESP32_VTPROTO_INTERFACE_H_