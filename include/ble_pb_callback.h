#ifndef BLE_PB_CALLBACK_H_
#define BLE_CB_CALLBACK_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <pb_decode.h>
#include <vtproto.pb.h>
#include <vtproto.h>
namespace tact
{
    namespace ble
    {

        class ReceiveVtprotoCallback : public BLECharacteristicCallbacks
        {
        public:
            ReceiveVtprotoCallback(tact::vtproto::MessageReceiver *receiver);
            void onWrite(BLECharacteristic *pCharacteristic);
            void onRead(BLECharacteristic *pCharacteristic);

        private:
            // tact::vtproto::MessageReceiver &message_receiver_;
            tact::vtproto::MessageReceiver *message_receiver_;
        };

    } // namespace ble

} // namespace tact

#endif // !BLE_PB_CALLBACK_H_