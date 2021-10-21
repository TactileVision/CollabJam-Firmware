#include "ble_playback_request_callback.h"
namespace tact {

namespace ble {

ReceivePlaybackRequestCallback::ReceivePlaybackRequestCallback(
    tact::vtproto::FileRequestReceiver* receiver)
    : receiver_(receiver) {
  buf_[0] = kPlaybackRequestMaxLength;
}
void ReceivePlaybackRequestCallback::onWrite(
    BLECharacteristic* pCharacteristic) {
  pb_istream_t istream_ = pb_istream_from_buffer(
      pCharacteristic->getData(), pCharacteristic->getValue().length());
  ReqPlayTactonFile ptf = ReqPlayTactonFile_init_default;
  ptf.filename.funcs.decode =
      ReceivePlaybackRequestCallback::callbackDecodeString;
  ptf.filename.arg = &buf_;
  if (!pb_decode_ex(&istream_, ReqPlayTactonFile_fields, &ptf,
                    PB_DECODE_DELIMITED)) {
#ifdef DEBUG_SERIAL
    Serial.print("Error decoding filename");
#endif  // !DEBUG_SERIAL
  }
  std::string filename((char*)&buf_[2], buf_[1]);
#ifdef DEBUG_SERIAL
  Serial.print("Requested playback for file: ");
  Serial.println(filename.c_str());
#endif  // !DEBUG_SERIAL
  receiver_->onFilename(filename);
}
void ReceivePlaybackRequestCallback::onRead(
    BLECharacteristic* pCharacteristic) {}

bool ReceivePlaybackRequestCallback::callbackDecodeString(
    pb_istream_t* stream, const pb_field_t* field, void** arg) {
  // https://github.com/nanopb/nanopb/blob/a9b7a2f91ba7dad78179500af1a0c241bad0b099/tests/callbacks/decode_callbacks.c#L10
  // TODO Check this flaky stuff out

  uint8_t* str = (uint8_t*)(*arg);
  uint8_t write_len = stream->bytes_left;
  // Check if max buffer length is big enough to store decoded message. Max
  // buffer length is stored at str[0]
  if (stream->bytes_left > (*str - 3)) {
    // Serial.println("Too small, truncating to available size");
    write_len = *str - 3;
    // Serial.println("Too small");
    // return false;
  }

  // Serial.println("DECODING");
  // Serial.printf("Writing %i of %i bytes\n", write_len, stream->bytes_left);

  // Store length of message in str[1]
  ++str;
  *str = write_len;

  // write message starting at str[2]
  ++str;
  // if (!pb_read(stream, str, stream->bytes_left))
  if (!pb_read(stream, str, write_len)) return false;

  // empty out stream
  if (stream->bytes_left > 0) {
    {
      if (!pb_read(stream, NULL, stream->bytes_left)) return false;
    }
  }

  // Serial.println(stream->bytes_left);
  // return stream->bytes_left == 0;
  return true;
}
}  // namespace ble

}  // namespace tact
