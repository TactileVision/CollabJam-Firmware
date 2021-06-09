#include <Arduino.h>
#include <ble_esp.h>
#include <ble_services.h>
#include <vtproto.h>
#include <vtproto_handler/immediate_output.h>

#include "ble_pb_callback.h"
#include "esp32_vtproto_interface.h"

namespace tact
{
  namespace vtproto
  {
    const uint16_t kBufferSize = 2048;
    uint8_t vt_message_buffer[kBufferSize];
  }
  namespace ble
  {
    const std::string kDeviceName = "ESP32 VTP";
  }

  tact::vtproto::HardwareInterface vtp_esp_interface(&setIntensity, kNumOfOutputs);
  tact::vtproto::ImmediateOutputMode immediate_output_mode(vtp_esp_interface);
  tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);
}

void setup()
{

#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  while (!Serial)
  {
    /* code */
  }
#endif

  BLEDevice::init(tact::ble::kDeviceName);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  // Setup Service and its Characteristics
  BLEService *service = server->createService(tact::ble::service_tactile_display::kServiceUuid, 1 + (tact::ble::service_tactile_display::kNumberOfCharacteristics * 5));
  pAdvertising->addServiceUUID(tact::ble::service_tactile_display::kServiceUuid);
  BLECharacteristic *characteristics[tact::ble::service_tactile_display::kNumberOfCharacteristics];
  createCharacteristicsFromDescription(service, characteristics, tact::ble::service_tactile_display::characteristic_descriptions, tact::ble::service_tactile_display::kNumberOfCharacteristics);

  // Assign Values to Characteristics
  characteristics[tact::ble::service_tactile_display::kCharacteristicNumberOfOutputs]->setValue((uint8_t *)&kNumOfOutputs, 1);
  characteristics[tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackState]->setValue("IDLE");
  // characteristics[tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setValue((uint16_t)0); // No need to write?
  //TODO Check if value of constant is equal to propagated value
  characteristics[tact::ble::service_tactile_display::kCharacteristicModeVtprotoMaxMsgLength]->setValue((uint16_t &)tact::vtproto::kBufferSize);

  // Assign Callbacks to Characteristics
  // characteristics[tact::ble::service_tactile_display::kCharacteristicModeVtprotoBuffer]->setCallbacks(new tact::ble::ReceiveVtprotoCallback());
  characteristics[tact::ble::service_tactile_display::kCharacteristicModeVtprotoBuffer]->setCallbacks(&tact::vtp_ble_callback);
  // characteristics[tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setCallbacks(new s);

  service->start();
  BLEDevice::startAdvertising();

#ifdef DEBUG_SERIAL
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
