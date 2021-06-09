#include "esp32_vtproto_interface.h"

uint8_t getPwmFromFloat(float value)
{
    uint16_t v = (uint16_t)(value * 256);
    return v < 255 ? v : 255;
}

void setIntensity(uint8_t channel, float intensity)
{
    if (channel > kNumOfOutputs)
    {

#ifndef DEBUG_SERIAL
#endif // !DEBUG_SERIAL
        Serial.println("Channel number to high, does not exsist");
        return;
    }
    analogWrite(kChannelToPinMap[channel], getPwmFromFloat(intensity));
}

void setFrequency(uint8_t channel, float intensity)
{
#ifdef DEBUG_SERIAL

    Serial.println("Setting frequency not supported with this device");
#endif // !DEBUG_SERIAL
}