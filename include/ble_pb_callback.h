#ifndef BLE_PB_CALLBACK_H_
#define BLE_PB_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <pb_decode.h>
#include <vtproto.h>
#include <vtproto.pb.h>

#include "interface/message_interface.h"
#include "tacton/tacton_store.h"
namespace tact {
namespace ble {

class ReceiveVtprotoCallback : public BLECharacteristicCallbacks {
 public:
  ReceiveVtprotoCallback(tact::vtproto::MessageReceiver* receiver);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);
  void changeMessageReviecer(tact::vtproto::MessageReceiver* receiver);

 private:
  tact::vtproto::Tacton tacton_;
  // tact::vtproto::MessageReceiver &message_receiver_;
  tact::vtproto::MessageReceiver* message_receiver_;
  // tact::vtproto::TactonStore* tacton_store_;
};

}  // namespace ble

}  // namespace tact

#endif  // !BLE_PB_CALLBACK_H_