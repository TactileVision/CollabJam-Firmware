#ifndef BLE_FREQ_HANDLER_H_
#define BLE_FREQ_HANDLER_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include "debug_util.h"

#include "hardware_interfaces/hardware_interface.h"
// #include "interface/message_interface.h"
// #include "tacton/tacton_store.h"

/**
 * Class that receives vtproto via ble and executes the instructions immediately
 */
class BLEFrequencyChangedHandler : public BLECharacteristicCallbacks {
 public:
  BLEFrequencyChangedHandler();
  ~BLEFrequencyChangedHandler();
  BLEFrequencyChangedHandler(HardwareInterface& h, uint8_t num_outputs,
                             TactileDisplayFrequencyInformation freq_info);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);
  HardwareInterface& output_;
  uint8_t num_outputs_;
  TactileDisplayFrequencyInformation freq_info_;
};

#endif  // !BLE_PB_CALLBACK_H_