#ifndef BLE_PLAYBACK_REQUEST_CALLBACK_H_
#define BLE_PLAYBACK_REQUEST_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <interface/file_interface.h>
#include <pb_decode.h>

#include "tactile_display.pb.h"

namespace tact {
namespace ble {
const uint8_t kPlaybackRequestMaxLength = 35;

class ReceivePlaybackRequestCallback : public BLECharacteristicCallbacks {
 public:
  ReceivePlaybackRequestCallback(tact::vtproto::FileRequestReceiver* receiver);
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);

 private:
  uint8_t buf_[kPlaybackRequestMaxLength] = {0};
  tact::vtproto::FileRequestReceiver* receiver_;
  static bool callbackDecodeString(pb_istream_t* stream,
                                   const pb_field_t* field, void** arg);
};

}  // namespace ble

}  // namespace tact

#endif  // !BLE_PLAYBACK_REQUEST_CALLBACK_H_