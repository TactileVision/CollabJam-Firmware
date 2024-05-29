#include <Arduino.h>
#include <BLEDevice.h>

#include "ble/ble_amp_handler.h"
#include "ble/ble_freq_handler.h"
#include "ble/ble_services.h"
#include "ble/ble_util.h"
#include "defines.h"

#if defined(BOARD_CJ0) || defined(BOARD_CJ1)
#include "hardware/uln_pinouts.h"
#include "hardware_interfaces/esp32_hw_interface.h"
// !END BOARD_CJ0 BOARD_CJ1
#endif
#ifdef BOARD_CJ2
#include "Wire.h"
#include "hardware_interfaces/cj2_hw_interface.h"
// !END BOARD_CJ2
#elif BOARD_CJ3
#include "Wire.h"
#include "hardware/multiplexed_drv.h"
#include "hardware_interfaces/drvs_hw_interface.h"
// !END BOARD_CJ3
#elif BOARD_CJ4
#include "Wire.h"
#include "hardware_interfaces/cj4_hw_interface.h"
// !END BOARD_CJ4
#endif


std::string pcb_name = PCB_NAME;
std::string pcb_version = PCB_VERSION;
std::string device_name = pcb_name + " " + pcb_version;

#if defined(BOARD_CJ0) || defined(BOARD_CJ1)
// TODO Fix number of outputs and pin map specification
TactileDisplayFrequencyInformation freq_info = {0, 0, 0};
TactileDisplayInformation td_info = {8, 0xFF, 0x00};
// GPIO with ULN2803A
EspVtprotoHardwareInterface hw_interface((uint8_t)
                                             config::display::kNumOfOutputs,
                                         (uint8_t*)config::display::kMotorPins);
// !END BOARD_CJ0 BOARD_CJ1
#endif
#ifdef BOARD_CJ2
// TODO Check Frequency ranges for DA7280
TactileDisplayFrequencyInformation freq_info = {25, 500, 170};
TactileDisplayInformation td_info = {4, 0x0F, 0x00};
CollabJamV2HardwareInterface hw_interface;
// !END BOARD_CJ2
#elif BOARD_CJ3
TactileDisplayFrequencyInformation freq_info = {50, 300, 170};
TactileDisplayInformation td_info = {5, 0x1F, 0x1F};
MultiplexedDrv drvs;
ActuatorConfig actuator_conf = {MotorType::kLRA, 1.0, 1.0,
                                freq_info.f_resonance};
DrvConfig drv_conf = {DRV2605_MODE_INTTRIG, 2, LoopMode::kOpenLoop, false};
MultiplexedDrvsInterface hw_interface(td_info.num_outputs);
// !END BOARD_CJ3
#elif BOARD_CJ4
TactileDisplayFrequencyInformation freq_info = {25, 500, 170};
TactileDisplayInformation td_info = {6, 0x3F, 0x3F};
CollabJamV4HardwareInterface hw_interface;
// !END BOARD_CJ4
#endif

BLEAmplitudeChangedHandler amp_handler(hw_interface, td_info.num_outputs);
BLEFrequencyChangedHandler freq_handler(hw_interface, td_info.num_outputs,
                                        freq_info);
uint32_t ms = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) {
  }
  delay(1000);
  Serial.println(FW_NAME);
  Serial.println(GIT_TAG);
  Serial.println(GIT_REV);
  auto device = "device: " + device_name;
  Serial.println(device_name.c_str());
#endif

  // Initialize Hardware
#ifdef BOARD_CJ3
  Wire.begin();
  delay(2500);
  drvs.initAllDrvs(actuator_conf, drv_conf);
  drvs.pingAllOutputs();
  hw_interface.init(&drvs, &actuator_conf, &drv_conf);
// !END BOARD_CJ3
#endif
#if defined(BOARD_CJ2) || defined(BOARD_CJ4)
  Wire.begin();
  delay(2500);
  hw_interface.init();
// !END BOARD_CJ2 || BOARD_CJ4
#endif

  // Initialize BLE
  BLEDevice::init(device_name);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());
  // BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
  // BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_SCAN);

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

  characteristics[tact::ble::vtproto_service::kChrFreqInformation]->setValue(
      (uint8_t*)&freq_info, 6);

  // Assign Callbacks to Characteristics
  characteristics[tact::ble::vtproto_service::kChrAmplitudeBuffer]
      ->setCallbacks(&amp_handler);
  characteristics[tact::ble::vtproto_service::kChrFreqBuffer]->setCallbacks(
      &freq_handler);

  service->start();
  BLEDevice::startAdvertising();

#ifdef DEBUG
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop() {}
