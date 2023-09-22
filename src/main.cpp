#include <Arduino.h>
#include <BLEDevice.h>

#include "Wire.h"
#include "ble/ble_services.h"
#include "ble/ble_util.h"
#include "ble_handler.h"
#include "config.h"

#ifdef DRVS
#include "drv/multiplexed_drv.h"
#include "hardware_interfaces/i2c_mp_drv_hw_interface.h"
#else
#include "hardware_interfaces/esp32_hw_interface.h"
#endif
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
#ifdef DRVS
tact::MultiplexedDrvsInterface vtp_esp_interface(
    (uint8_t)config::display::kNumOfOutputs,
    (uint8_t*)config::display::kMotorPins);

MultiplexedDrv drvs;
tact::ActuatorConfig actuator_conf = {tact::MotorType::kLRA, 1.0, 1.0, 170};
tact::DrvConfig drv_conf = {DRV2605_MODE_INTTRIG, 2, tact::LoopMode::kOpenLoop,
                            false};
tact::TactileDisplayInformation td_info = {5, 0x17, 0x17};
#else
// GPIO with ULN2803A
EspVtprotoHardwareInterface vtp_esp_interface(
    (uint8_t)config::display::kNumOfOutputs,
    (uint8_t*)config::display::kMotorPins);
tact::TactileDisplayInformation td_info = {8, 0xFF, 0x00};
#endif
tact::ble::BLEVibrationHandler vibro_handler(vtp_esp_interface);
uint32_t ms = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("Vtpoto Streaming Device in DEBUG Mode");
#endif

#ifdef DRVS
  Wire.begin();
  delay(2500);
  drvs.initAllDrvs(actuator_conf, drv_conf);
  drvs.pingAllOutputs();
  vtp_esp_interface.init(&drvs, &actuator_conf, &drv_conf);
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
      (uint8_t*)&td_info.num_outputs, 4);
  characteristics[tact::ble::vtproto_service::kChrOutputCanAmplitude]->setValue(
      (uint8_t*)&td_info.can_change_amplitude, 4);
  characteristics[tact::ble::vtproto_service::kChrOutputCanFrequency]->setValue(
      (uint8_t*)&td_info.can_change_frequency, 4);

  // Assign Callbacks to Characteristics
  characteristics[tact::ble::vtproto_service::kChrAmplitudeBuffer]
      ->setCallbacks(&vibro_handler);
  characteristics[tact::ble::vtproto_service::kChrFreqBuffer]->setCallbacks(
      &vibro_handler);

  service->start();
  BLEDevice::startAdvertising();

#ifdef DEBUG
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop() {
  // action happend in ble_handler.cc as well as in the hardware interfaces
}
