#ifndef BLE_CALLBACK_H_
#define BLE_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <cppQueue.h>

#include "hardware_interfaces/hardware_interface.h"
#include "hardware_interfaces/pwm_state.h"
#include "vtproto_callback.h"
// #include "interface/message_interface.h"
// #include "tacton/tacton_store.h"
namespace tact {
namespace ble {
/*
class InstructionQueue {
 public:
  Instruction instructions[8];
  Instruction* addToQueue();
  Instruction* getNextInQueue();
  uint8_t size_ = 0;
  uint8_t push_ = 0;
  uint8_t pop_ = 0;
} */

// TODO Change to value from config
typedef struct DisplayUpdate {
  uint8_t values[5];
} DisplayUpdate;

/**
 * Class that receives vtproto via ble and executes the instructions immediately
 */
class BLEVibrationHandler : public BLECharacteristicCallbacks {
 public:
  BLEVibrationHandler(tact::vtproto::HardwareInterface& h);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);
  // void attachHardwareInterface(tact::vtproto::HardwareInterface& h);
  cppQueue instruction_queue_;

  void updateDisplay(uint8_t* values, uint8_t length);

 private:
  /// @brief  Where to display the received instructions on
  tact::vtproto::HardwareInterface& output_;
  tact::vtproto::PwmState compensated_state_, received_state_;

  uint8_t active_channels_ = 0;
};

}  // namespace ble

}  // namespace tact

#endif  // !BLE_PB_CALLBACK_H_