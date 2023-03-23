// #ifndef BLE_PLAYBACK_REQUEST_CALLBACK_H_
// #define BLE_PLAYBACK_REQUEST_CALLBACK_H_
// #include <Arduino.h>
// #include <BLECharacteristic.h>
// #include <interface/file_interface.h>
// #include <pb_decode.h>

// #include "config.h"
// #include "tactile_display.pb.h"

// namespace tact {
// namespace ble {

// class ReceivePlaybackRequestCallback : public BLECharacteristicCallbacks {
//  public:
//   ReceivePlaybackRequestCallback(tact::vtproto::FileRequestReceiver* receiver);
//   void onWrite(BLECharacteristic* pCharacteristic);
//   void onRead(BLECharacteristic* pCharacteristic);

//  private:
//   uint8_t buf_[config::ble::kReceivePlaybackRequestBufSize] = {0};
//   tact::vtproto::FileRequestReceiver* receiver_;
//   static bool callbackDecodeString(pb_istream_t* stream,
//                                    const pb_field_t* field, void** arg);
// };

// }  // namespace ble

// }  // namespace tact

// #endif  // !BLE_PLAYBACK_REQUEST_CALLBACK_H_