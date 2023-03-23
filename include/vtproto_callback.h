#ifndef VTPROTO_CALLBACK_H_
#define VTPROTO_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <cppQueue.h>
#include <pb_decode.h>
#include <vtproto.h>
#include <vtproto.pb.h>

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

typedef struct InstructionQueueItem {
  Instruction instruction;
  tact::vtproto::Group group;
} InstructionQueueItem;

/**
 * Class that receives vtproto via ble and executes the instructions immediately
 */
class VtprotoHandler : public BLECharacteristicCallbacks {
 public:
  VtprotoHandler(tact::vtproto::HardwareInterface& h);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);
  // void attachHardwareInterface(tact::vtproto::HardwareInterface& h);
  cppQueue instruction_queue_;

  void executeInstruction(Instruction& i);

 private:
  /// @brief  Where to display the received instructions on
  tact::vtproto::HardwareInterface& output_;

  /// @brief  Handles which vproto groups were already created and which pins
  /// are associated with that group
  Instruction instruction_ = Instruction_init_default;
  Instruction instructions[8];

  tact::vtproto::GroupManager group_manager_;
  tact::vtproto::PwmState compensated_state_, received_state_;
  uint8_t active_channels_ = 0;

  void setInstantParameter(InstInstantlySetParameter instruction);
};

}  // namespace ble

}  // namespace tact

#endif  // !BLE_PB_CALLBACK_H_