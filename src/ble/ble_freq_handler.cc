#include "ble/ble_freq_handler.h"

BLEFrequencyChangedHandler::BLEFrequencyChangedHandler(
    HardwareInterface &h, uint8_t num_outputs,
    TactileDisplayFrequencyInformation freq_info)
    : output_(h), num_outputs_(num_outputs), freq_info_(freq_info) {}

BLEFrequencyChangedHandler::~BLEFrequencyChangedHandler() {}

void BLEFrequencyChangedHandler::onWrite(BLECharacteristic *pCharacteristic) {
  DEBUG_PRINTF("[BLE/FREQ]Received message with length %i\n",
               pCharacteristic->getValue().length());
  uint8_t *data = pCharacteristic->getData();
  uint8_t len = pCharacteristic->getValue().length() > (2 * num_outputs_)
                    ? (2 * num_outputs_)
                    : pCharacteristic->getValue().length();

  // UPDATE DISPLAY
  uint8_t dev_index = 0;
  for (size_t i = 0; i < len; i = i + 2) {
    // 255 is used as a symbol with the meaning "Don't change this channel"
    // i = 0, 2, 4, ...
    DEBUG_PRINTF("[BLE/FREQ] Received value\n");
    uint16_t freq = data[i] | (data[i + 1] << 8);
    DEBUG_PRINTLN(data[i]);
    DEBUG_PRINTLN(data[i + 1]);
    if (freq >= freq_info_.f_min && freq <= freq_info_.f_max && freq != 0) {
      // output_.setIntensity(i, (uint8_t)data[i]);
      DEBUG_PRINTF("[BLE/FREQ] Set actor to %u Hz\n", freq);
      output_.setFrequency(dev_index, freq);
    }
    ++dev_index;
  }
}

void BLEFrequencyChangedHandler::onRead(BLECharacteristic *pCharacteristic) {}
