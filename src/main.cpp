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
#include "tactile_display.h"
#include "tactile_display.pb.h"
#include "tacton_player_esp.h"

namespace tact {
namespace vtproto {
const uint16_t kBufferSize = 2048;
uint8_t vt_message_buffer[kBufferSize];
TactonStore tacton_store;

// Tacton tacton;
}  // namespace vtproto
namespace display {

const uint16_t kConfigChangeBufferSize = 255;
uint8_t config_change_buffer[kConfigChangeBufferSize];

const uint8_t kNumOfOutputs = 8;
uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25, 33, 32, 19, 21};

const uint8_t kAvailableOutputModesLength = 2;
const uint8_t kAvailableMotorTypesLength = 2;
OutputMode available_ouput_modes[kAvailableOutputModesLength] = {
    OutputMode_VTPROTO_TACTON, OutputMode_VTPROTO_REALTIME};
MotorType available_motor_types[kAvailableMotorTypesLength] = {
    MotorType_ERM, MotorType_PNEUMATIC};
OutputMode current_output_mode = OutputMode_VTPROTO_REALTIME;
ChannelConfig channel_configs[kNumOfOutputs] = {
    {1, MotorType::MotorType_ERM, false, 0},
    {2, MotorType::MotorType_ERM, false, 0},
    {3, MotorType::MotorType_ERM, false, 0},
    {4, MotorType::MotorType_ERM, false, 0},
    {5, MotorType::MotorType_ERM, false, 0},
    {6, MotorType::MotorType_ERM, false, 0},
    {7, MotorType::MotorType_ERM, false, 0},
    {8, MotorType::MotorType_ERM, false, 0},
};

tact::vtproto::encode::DisplayConfigEncoder display_config_encoder(
    OutputMode::OutputMode_VTPROTO_REALTIME, tact::display::channel_configs,
    tact::display::kNumOfOutputs);
OutputMode last_active_mode = OutputMode_VTPROTO_REALTIME;

tact::vtproto::encode::ConfigOptionsEncoder config_options_encoder(
    tact::display::available_ouput_modes,
    tact::display::kAvailableOutputModesLength,
    tact::display::available_motor_types,
    tact::display::kAvailableMotorTypesLength, false);
}  // namespace display
namespace ble {
const std::string kDeviceName = "ESP32 VTP";
tact::ble::ConfigOptionsCallbackHandler config_options_callback_handler(
    &tact::display::display_config_encoder,
    tact::display::config_options_encoder.getAvalilableConfigurationOptions());
}  // namespace ble

EspVtprotoHardwareInterface vtp_esp_interface(
    (uint8_t)tact::display::kNumOfOutputs, tact::display::kMotorPins);
tact::vtproto::TactonPlayerESP tacton_player_esp(vtp_esp_interface);

tact::vtproto::TactonReceiver tacton_receiver(
    tact::vtproto::tacton_store.getNewTacton());

tact::vtproto::ImmediateOutputMode immediate_output_mode(vtp_esp_interface);
// tact::vtproto::ReceiveAndPlayTactonMode rec_and_play(vtp_esp_interface);
// tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);
tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);
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
          ->setValue((uint8_t*)&tact::display::kNumOfOutputs, 1);

  characteristics
      [tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackState]
          ->setValue("IDLE");

  // characteristics[tact::ble::service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setValue((uint16_t)0);
  // // No need to write?
  // TODO Check if value of constant is equal to propagated value

  characteristics[tact::ble::service_tactile_display::
                      kCharacteristicModeVtprotoMaxMsgLength]
      ->setValue((uint16_t&)tact::vtproto::kBufferSize);

  // Display config characteristic
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicDisplayConfig]
          ->setValue(
              tact::display::display_config_encoder.getEncodedMessage(),
              tact::display::display_config_encoder.getEncodedMessageLength());

  // Available display config options buffer
  // Available channel configs buffer
  characteristics[tact::ble::service_tactile_display::
                      kCharacteristicConfigAvailableOptions]
      ->setValue(
          tact::display::config_options_encoder.getEncodedMessage(),
          tact::display::config_options_encoder.getEncodedMessageLength());

  characteristics[tact::ble::service_tactile_display::
                      kCharacteristicConfigChangeBufferMaxLength]
      ->setValue((uint16_t&)tact::display::kConfigChangeBufferSize);
  // Assign Callbacks to Characteristics
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicModeVtprotoBuffer]
          ->setCallbacks(&tact::vtp_ble_callback);

  // Assign displayConfig characteristic to handler instance and instance to
  tact::ble::config_options_callback_handler.setBleCharacteristic(
      characteristics
          [tact::ble::service_tactile_display::kCharacteristicDisplayConfig]);
  characteristics
      [tact::ble::service_tactile_display::kCharacteristicConfigChangeBuffer]
          ->setCallbacks(&tact::ble::config_options_callback_handler);

  service->start();

  BLEDevice::startAdvertising();

#ifdef DEBUG_SERIAL
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop() {
  // Check current mode
  if (tact::display::last_active_mode !=
      tact::display::display_config_encoder.getDisplayConfig()->output_mode) {
    if (tact::display::last_active_mode == OutputMode_VTPROTO_TACTON) {
#ifdef DEBUG_SERIAL
      Serial.println("Switching to Realtime Mode");
#endif
      tact::display::last_active_mode = OutputMode_VTPROTO_REALTIME;
      tact::vtp_ble_callback.changeMessageReviecer(
          &tact::immediate_output_mode);

    } else {
#ifdef DEBUG_SERIAL
      Serial.println("Switching to Tacton Mode");
#endif
      tact::display::last_active_mode = OutputMode_VTPROTO_TACTON;
      tact::tacton_receiver.reset(tact::vtproto::tacton_store.getNewTacton());
      tact::vtp_ble_callback.changeMessageReviecer(&tact::tacton_receiver);
      // Reset TactonReceiver?
    }
  }

  if (tact::display::display_config_encoder.getDisplayConfig()->output_mode ==
      OutputMode_VTPROTO_TACTON) {
    // TODO Check if device is playing or not
    if (tact::tacton_receiver.hasReceivedTacton()) {
#ifdef DEBUG_SERIAL
      Serial.println("Received a tacton!");
#endif
      // Play back tacton
      tact::tacton_player_esp.play(tact::vtproto::tacton_store.getLastTacton());
      tact::tacton_receiver.reset(tact::vtproto::tacton_store.getNewTacton());
    }

  } else {
  }
}
