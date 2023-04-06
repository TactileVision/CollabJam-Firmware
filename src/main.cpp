#include <Arduino.h>
#include <BLEDevice.h>

#include "ble/ble_services.h"
#include "ble/ble_util.h"
#include "ble_handler.h"
#include "config.h"
#include "hardware_interfaces/esp32_hw_interface.h"
#include "vtproto_callback.h"
// #include <cppQueue.h>
/*

 TODO: Remove all external references
 TODO: Focus firmware only on the direct playback of received instructions
 TODO: Move BLE Init to its own file/class
 TODO: Switch to NimBLE
 TODO: Make a BLE Class wrapper
 TODO: Refactore namespaces

*/
namespace {

// GPIO with ULN2803A
EspVtprotoHardwareInterface vtp_esp_interface(
    (uint8_t)config::display::kNumOfOutputs,
    (uint8_t*)config::display::kMotorPins);

// tact::ble::VtprotoHandler vtproto_handler(vtp_esp_interface);
tact::ble::BLEVibrationHandler vibro_handler(vtp_esp_interface);

uint32_t ms = 0;

}  // namespace

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Vtpoto Streaming Device in DEBUG Mode");
#endif
  // Initialize BLE
  BLEDevice::init(config::ble::kDeviceName);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());
  BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
  BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_SCAN);

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(
      0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  // Setup Service and its Characteristics
  BLEService* service = server->createService(
      tact::ble::vtproto_service::kServiceUuid,
      1 + (tact::ble::vtproto_service::kNumberOfCharacteristics * 5));
  pAdvertising->addServiceUUID(tact::ble::vtproto_service::kServiceUuid);
  BLECharacteristic*
      characteristics[tact::ble::vtproto_service::kNumberOfCharacteristics];
  createCharacteristicsFromDescription(
      service, characteristics,
      tact::ble::vtproto_service::characteristic_descriptions,
      tact::ble::vtproto_service::kNumberOfCharacteristics);

  // Assign Values to Characteristics
  characteristics[tact::ble::vtproto_service::kChrNumberOfOutputs]->setValue(
      (uint8_t*)&config::display::kNumOfOutputs, 1);

  // Not used yet
  characteristics[tact::ble::vtproto_service::kChrDisplayPlaybackState]
      ->setValue("IDLE");

  // characteristics[tact::ble::vtproto_service::kChrDisplayPlaybackRequestStop]->setValue((uint16_t)0);
  // // No need to write?
  // TODO Check if value of constant is equal to propagated value

  characteristics[tact::ble::vtproto_service::kChrModeVtprotoMaxMsgLength]
      ->setValue((uint16_t&)config::ble::kVtprotoBufSize);

  // Assign Callbacks to Characteristics
  characteristics[tact::ble::vtproto_service::kChrModeVtprotoBuffer]
      ->setCallbacks(&vibro_handler);

  service->start();
  BLEDevice::startAdvertising();

#ifdef DEBUG
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
  ms = millis();
}

void loop() {
  if (!vibro_handler.instruction_queue_.isEmpty()) {
    tact::ble::DisplayUpdate du;
    vibro_handler.instruction_queue_.pop(&du);
    vibro_handler.updateDisplay(du.values, 5);
#ifdef DEBUG
    Serial.printf("%d ms since last execution\n", millis() - ms);
#endif
    ms = millis();
  };
}
