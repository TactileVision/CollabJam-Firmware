#include <Arduino.h>
#include <BLEDevice.h>
#include "defines.h"
#include "ble/ble_amp_handler.h"
#include "ble/ble_freq_handler.h"
#include "ble/ble_services.h"
#include "ble/ble_util.h"



#ifdef DRVS
#include "Wire.h"
#include "hardware/multiplexed_drv.h"
#include "hardware_interfaces/drvs_hw_interface.h"
#elif ISSDRIVERPCB
#include "Wire.h"
#include "hardware/multiplexed_drv.h"
#include "hardware_interfaces/iss_pcb_hw_interface.h"
#elif BOARD_CJ2
#include "Wire.h"
#include "hardware_interfaces/cj2_hw_interface.h"
#elif BOARD_CJ4
#include "Wire.h"
#include "hardware_interfaces/cj4_hw_interface.h"
#else
#include "hardware/uln_pinouts.h"
#include "hardware_interfaces/esp32_hw_interface.h"
#endif

#ifdef DRVS
std::string device_name = "TB_LRA_HEADBAND";
TactileDisplayFrequencyInformation freq_info = {50, 300, 170};
TactileDisplayInformation td_info = {5, 0x1F, 0x1F};

MultiplexedDrv drvs;
ActuatorConfig actuator_conf = {MotorType::kLRA, 1.0, 1.0,
                                freq_info.f_resonance};
DrvConfig drv_conf = {DRV2605_MODE_INTTRIG, 2, LoopMode::kOpenLoop, false};

MultiplexedDrvsInterface hw_interface(td_info.num_outputs);

#elif ISSDRIVERPCB
std::string device_name = "TB_ISS_PCB";
TactileDisplayFrequencyInformation freq_info = {50, 300, 170};
TactileDisplayInformation td_info = {32, 0xFFFFFFFF, 0xFFFFFFFF};
MultiplexedDrv drvs[4];
ActuatorConfig actuator_conf = {MotorType::kLRA, 3.0, 3.0,
                                freq_info.f_resonance};
DrvConfig drv_conf = {DRV2605_MODE_INTTRIG, 2, LoopMode::kOpenLoop, false};
IssPcbHardwareInterface hw_interface(td_info.num_outputs);
#elif BOARD_CJ2
std::string device_name = "CollabJam Ver.2 II";
// TODO Check Frequency ranges for DA7280
TactileDisplayFrequencyInformation freq_info = {25, 500, 170};
TactileDisplayInformation td_info = {4, 0x0F, 0x00};
CollabJamV2HardwareInterface hw_interface;
#elif BOARD_CJ4
std::string device_name = "CollabJam Ver.4";
TactileDisplayFrequencyInformation freq_info = {25, 500, 170};
TactileDisplayInformation td_info = {6, 0x3F, 0x3F};
CollabJamV4HardwareInterface hw_interface;
#else
std::string device_name = "TB_ERM_CJ";
// TODO Fix number of outputs and pin map specification
TactileDisplayFrequencyInformation freq_info = {0, 0, 0};
TactileDisplayInformation td_info = {8, 0xFF, 0x00};

// GPIO with ULN2803A
EspVtprotoHardwareInterface hw_interface((uint8_t)
                                             config::display::kNumOfOutputs,
                                         (uint8_t*)config::display::kMotorPins);
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
  Serial.println("Vtpoto Streaming Device in DEBUG Mode");
#endif


#ifdef DRVS
  Wire.begin();
  delay(2500);
  drvs.initAllDrvs(actuator_conf, drv_conf);
  drvs.pingAllOutputs();
  hw_interface.init(&drvs, &actuator_conf, &drv_conf);
#elif ISSDRIVERPCB
  Wire.begin();
  pinMode(25, OUTPUT);  // sets the digital pin 13 as output
  pinMode(26, OUTPUT);  // sets the digital pin 13 as output
  pinMode(27, OUTPUT);  // sets the digital pin 13 as output

  digitalWrite(25, HIGH);  // Gloabal enable pin
  digitalWrite(26, HIGH);  // Gloabal enable pin
  digitalWrite(27, HIGH);  // Gloabal enable pin
  delay(2500);
  for (uint8_t i = 0; i < 4; i++) {
    // TODO Double check addresses!
    drvs[i].setMultiplexerAddress(0x71 + i);
    drvs[i].initAllDrvs(actuator_conf, drv_conf);
    drvs[i].pingAllOutputs();
    delay(1000);
  }

  hw_interface.init(drvs, &actuator_conf, &drv_conf);
#elif BOARD_CJ2 || BOARD_CJ4
  Wire.begin();
  delay(2500);
  hw_interface.init();
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

void loop() {
  // action happend in ble_amp_handler.cc as well as in the hardware interfaces
}
