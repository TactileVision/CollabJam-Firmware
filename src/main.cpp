#include <Arduino.h>
#include <ble_esp.h>
#include <ble_services.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <vtproto.pb.h>
#include <vtproto.h>
#include "ble_pb_callback.h"
#include "config.h"

namespace tact
{
  namespace vtproto
  {
    const uint16_t kBufferSize = 2048;
    uint8_t vt_message_buffer[kBufferSize];

  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    /* code */
  }
  using namespace tact::ble;
  BLEDevice::init("device name");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  // Setup Service and its Characteristics
  BLEService *service = server->createService(service_tactile_display::kServiceUuid, 1 + (service_tactile_display::kNumberOfCharacteristics * 5));
  pAdvertising->addServiceUUID(service_tactile_display::kServiceUuid);
  BLECharacteristic *characteristics[service_tactile_display::kNumberOfCharacteristics];
  createCharacteristicsFromDescription(service, characteristics, service_tactile_display::characteristic_descriptions, service_tactile_display::kNumberOfCharacteristics);

  // Assign Values to Characteristics
  characteristics[service_tactile_display::kCharacteristicNumberOfOutputs]->setValue((uint8_t *)&tact::display::number_of_motors, 1);
  characteristics[service_tactile_display::kCharacteristicDisplayPlaybackState]->setValue("IDLE");
  // characteristics[service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setValue((uint16_t)0); // No need to write?
  //TODO Check if value of constant is equal to propagated value
  characteristics[service_tactile_display::kCharacteristicModeVtprotoMaxMsgLength]->setValue((uint16_t &)tact::vtproto::kBufferSize);

  // Assign Callbacks to Characteristics
  characteristics[service_tactile_display::kCharacteristicModeVtprotoBuffer]->setCallbacks(new tact::ble::VtprotoImidiateExecutionCallback());
  // characteristics[service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setCallbacks(new s);

  service->start();
  BLEDevice::startAdvertising();
  Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop()
{
  // put your main code here, to run repeatedly:
}
