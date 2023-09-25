#ifndef BLE_CALLBACK_H_
#define BLE_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>

#include "hardware_interfaces/hardware_interface.h"
// #include "interface/message_interface.h"
// #include "tacton/tacton_store.h"

/**
 * Class that receives vtproto via ble and executes the instructions immediately
 */
class BLEAmplitudeChangedHandler : public BLECharacteristicCallbacks {
 public:
  BLEAmplitudeChangedHandler();
  ~BLEAmplitudeChangedHandler();
  BLEAmplitudeChangedHandler(HardwareInterface& h, uint8_t num_outputs);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);
  HardwareInterface& output_;
  uint8_t num_outputs_;
};

#endif  // !BLE_PB_CALLBACK_H_