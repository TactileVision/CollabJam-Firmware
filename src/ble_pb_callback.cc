#include "ble_pb_callback.h"

namespace tact
{
    namespace ble
    {
        ReceiveVtprotoCallback::ReceiveVtprotoCallback(tact::vtproto::MessageReceiver *receiver) : message_receiver_(receiver){};
        void ReceiveVtprotoCallback::onWrite(BLECharacteristic *pCharacteristic)
        {
            /*
            Read Data from buffer
            Decode data with pb_decode_ex, only expect instructions
            execute instructions after reveicinmg them
            */
#ifdef DEBUG_SERIAL
            Serial.printf("Received message %s (length %i)\n", pCharacteristic->getValue(), pCharacteristic->getValue().length());
            for (size_t i = 0; i < pCharacteristic->getValue().length(); i++)
            {
                Serial.printf("%X ", pCharacteristic->getData()[i]);
            }
            Serial.println();
#endif

            pb_istream_t istream_ = pb_istream_from_buffer(pCharacteristic->getData(), pCharacteristic->getValue().length());
            Instruction instruction_ = Instruction_init_default;
            tact::vtproto::Group g;
            instruction_.cb_concrete_instruction.funcs.decode = tact::vtproto::decode::callbackInstruction;
            instruction_.cb_concrete_instruction.arg = &g;
            while (istream_.bytes_left > 0)
            {
                if (!pb_decode_ex(&istream_, Instruction_fields, &instruction_, PB_DECODE_DELIMITED))
                {

#ifdef DEBUG_SERIAL
                    Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&istream_));
#endif
                }
                // tact::vtproto::executeInstruction(instruction_);
                message_receiver_->onInstruction(instruction_);
            }
        }
        void ReceiveVtprotoCallback::onRead(BLECharacteristic *pCharacteristic)
        {
        }
    }
}