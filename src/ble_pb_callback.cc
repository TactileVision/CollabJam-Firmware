#include "ble_pb_callback.h"

namespace tact {
namespace ble {
// ReceiveVtprotoCallback::ReceiveVtprotoCallback(
// tact::vtproto::MessageReceiver *receiver) : message_receiver_(receiver){};
ReceiveVtprotoCallback::ReceiveVtprotoCallback(
    tact::vtproto::MessageReceiver *receiver) {
  this->message_receiver_ = receiver;
}

void ReceiveVtprotoCallback::onWrite(BLECharacteristic *pCharacteristic) {
  /*
  Read Data from buffer
  Decode data with pb_decode_ex, only expect instructions
  execute instructions after receivicing them
  */
#ifdef DEBUG
  Serial.printf("Received message with length %i\n",
                pCharacteristic->getValue().length());
  // for (size_t i = 0; i < pCharacteristic->getValue().length(); i++) {
  //   Serial.printf("%X ", pCharacteristic->getData()[i]);
  // }
  // Serial.println();
#endif

  pb_istream_t istream_ = pb_istream_from_buffer(
      pCharacteristic->getData(), pCharacteristic->getValue().length());

  while (istream_.bytes_left > 0) {
    if (message_receiver_->getExpectedMessageType() ==
        tact::vtproto::ExpectedMessage::kFileHeader) {
      this->tacton_.header_ = tact::vtproto::TactonHeader();
      this->tacton_.header_.file_header_ = tact::vtproto::decode::getHeader(
          this->tacton_.header_.pattern_name_, this->tacton_.header_.author_);

      if (!pb_decode_ex(&istream_, FileHeader_fields,
                        &this->tacton_.header_.file_header_,
                        PB_DECODE_DELIMITED | PB_DECODE_NOINIT)) {
#ifdef DEBUG
        Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&istream_));
#endif
      }
      // message_receiver_->onHeader(fh);
      message_receiver_->onHeader(this->tacton_.header_);
    } else {
#ifdef DEBUG
      Serial.println("Decoding instruction");
#endif
      Instruction instruction = Instruction_init_default;
      tact::vtproto::Group g;

      instruction.cb_concrete_instruction.funcs.decode =
          tact::vtproto::decode::callbackInstruction;

      instruction.cb_concrete_instruction.arg = &g;
      if (!pb_decode_ex(&istream_, Instruction_fields, &instruction,
                        PB_DECODE_DELIMITED | PB_DECODE_NOINIT)) {
#ifdef DEBUG
        Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&istream_));
#endif
      }
      message_receiver_->onInstruction(instruction);
    }
  }
}
void ReceiveVtprotoCallback::onRead(BLECharacteristic *pCharacteristic) {}
void ReceiveVtprotoCallback::changeMessageReviecer(
    tact::vtproto::MessageReceiver *receiver) {
  message_receiver_ = receiver;
};
}  // namespace ble
}  // namespace tact