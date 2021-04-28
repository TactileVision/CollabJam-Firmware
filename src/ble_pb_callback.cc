#include "ble_pb_callback.h"

namespace tact
{
    namespace ble
    {
        void VtprotoImidiateExecutionCallback::onWrite(BLECharacteristic *pCharacteristic)
        {
            /*
            Read Data from buffer
            Decode data with pb_decode_ex, only expect instructions
            execute instructions after reveicinmg them
            */
            Serial.printf("Received message %s (length %i)\n", pCharacteristic->getValue(), pCharacteristic->getValue().length());
            for (size_t i = 0; i < pCharacteristic->getValue().length(); i++)
            {
                Serial.printf("%X ", pCharacteristic->getData()[i]);
            }
            Serial.println();

            pb_istream_t istream_ = pb_istream_from_buffer(pCharacteristic->getData(), pCharacteristic->getValue().length());
            Instruction instruction_ = Instruction_init_default;
            tact::vtproto::group_t g;
            instruction_.cb_concrete_instruction.funcs.decode = tact::vtproto::decode::callbackInstruction;
            instruction_.cb_concrete_instruction.arg = &g;
            while (istream_.bytes_left > 0)
            {
                if (!pb_decode_ex(&istream_, Instruction_fields, &instruction_, PB_DECODE_DELIMITED))
                {
                    Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&istream_));
                }
                /* code */
                tact::vtproto::executeInstruction(instruction_);
            }
        }
        void VtprotoImidiateExecutionCallback::onRead(BLECharacteristic *pCharacteristic)
        {
        }
    }
}