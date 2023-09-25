#include "ble/ble_amp_handler.h"

BLEAmplitudeChangedHandler::BLEAmplitudeChangedHandler(
    HardwareInterface &h, uint8_t num_outputs)
    : output_(h), num_outputs_(num_outputs) {}

BLEAmplitudeChangedHandler::~BLEAmplitudeChangedHandler() {}

void BLEAmplitudeChangedHandler::onWrite(BLECharacteristic *pCharacteristic) {
#ifdef DEBUG
  Serial.printf("Received message with length %i\n",
                pCharacteristic->getValue().length());
#endif
  uint8_t *data = pCharacteristic->getData();
  uint8_t len = pCharacteristic->getValue().length() > 5
                    ? 5
                    : pCharacteristic->getValue().length();

  // UPDATE DISPLAY
  for (size_t i = 0; i < len; i++) {
    // 255 is used as a symbol with the meaning "Don't change this channel"
    if (data[i] != 255) {
      output_.setIntensity(i, (uint8_t)data[i]);
    }
  }
}

void BLEAmplitudeChangedHandler::onRead(BLECharacteristic *pCharacteristic) {}
