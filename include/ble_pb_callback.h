#ifndef BLE_PB_CALLBACK_H_
#define BLE_CB_CALLBACK_H_
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2904.h>
#include <ble.h>
#include <pb_decode.h>
#include <vtproto.pb.h>
#include <vtproto.h>
#include <vtproto_esp32.h>
namespace tact
{
    namespace ble
    {

        class VtprotoImidiateExecutionCallback : public BLECharacteristicCallbacks
        {

            void onWrite(BLECharacteristic *pCharacteristic);
            void onRead(BLECharacteristic *pCharacteristic);
        };

    } // namespace ble

} // namespace tact

#endif // !BLE_PB_CALLBACK_H_