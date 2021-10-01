#include <Arduino.h>
#include <ble_esp.h>
#include <ble_services.h>
#include <vtproto.h>
#include <vtproto_handler/immediate_output.h>
// #include <vtproto_handler/tacton_output.h>
#include <tacton/tacton_store.h>
#include <vtproto_handler/tacton_receiver.h>

#include "ble_pb_callback.h"
#include "esp32_vtproto_interface.h"
#include "tacton_player_esp.h"
namespace tact {
namespace vtproto {
const uint16_t kBufferSize = 2048;
uint8_t vt_message_buffer[kBufferSize];
TactonStore tacton_store;
// Tacton tacton;

}  // namespace vtproto
namespace ble {
const std::string kDeviceName = "ESP32 VTP";
// char authorBuf[130] = {0};
// char patterNameBuf[130] = {0};
}  // namespace ble

const uint8_t kNumOfOutputs = 8;
uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25, 33, 32, 19, 21};

// tact::vtproto::HardwareInterface vtp_esp_interface(&setIntensity,
//                                                    kNumOfOutputs);
EspVtprotoHardwareInterface vtp_esp_interface((uint8_t)kNumOfOutputs,
                                              kMotorPins);
tact::vtproto::TactonPlayerESP tacton_player_esp(vtp_esp_interface);

// tact::vtproto::TactonReceiver tacton_receiver(tact::vtproto::tacton_store,
//                                               tacton_player_esp);

tact::vtproto::TactonReceiver tacton_receiver(
    tact::vtproto::tacton_store.getNewTacton());

tact::vtproto::ImmediateOutputMode immediate_output_mode(vtp_esp_interface);
// tact::vtproto::ReceiveAndPlayTactonMode rec_and_play(vtp_esp_interface);
// tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);
tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&tacton_receiver);
}  // namespace tact

void setup() {
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  while (!Serial) {
  }
#endif

  BLEDevice::init(tact::ble::kDeviceName);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(
      0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  // Setup Service and its Characteristics
  BLEService* service = server->createService(
      tact::ble::service_tactile_display::kServiceUuid,
      1 + (tact::ble::service_tactile_display::kNumberOfCharacteristics * 5));
  pAdvertising->addServiceUUID(
      tact::ble::service_tactile_display::kServiceUuid);
  BLECharacteristic* characteristics
      [tact::ble::service_tactile_display::kNumberOfCharacteristics];
  createCharacteristicsFromDescription(
      service, characteristics,
      tact::ble::service_tactile_display::characteristic_descriptions,
      tact::ble::service_tactile_display::kNumberOfCharacteristics);

  // Assign Values to Characteristics
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicNumberOfOutputs]
          ->setValue((uint8_t*)&tact::kNumOfOutputs, 1);
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackState]
          ->setValue("IDLE");
  // characteristics[tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setValue((uint16_t)0);
  // // No need to write?
  // TODO Check if value of constant is equal to propagated value
  characteristics[tact::ble::service_tactile_display::
                      kCharacteristicModeVtprotoMaxMsgLength]
      ->setValue((uint16_t&)tact::vtproto::kBufferSize);

  // Assign Callbacks to Characteristics
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicModeVtprotoBuffer]
          ->setCallbacks(&tact::vtp_ble_callback);

  service->start();
  BLEDevice::startAdvertising();

#ifdef DEBUG_SERIAL
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  // TODO Check if device is playing or not
  if (tact::tacton_receiver.hasReceivedTacton()) {
#ifdef DEBUG_SERIAL
    Serial.println("Received a tacton!");
#endif
    // Play back tacton
    tact::tacton_player_esp.play(tact::vtproto::tacton_store.getLastTacton());

    tact::tacton_receiver.reset(tact::vtproto::tacton_store.getNewTacton());
  }
}
