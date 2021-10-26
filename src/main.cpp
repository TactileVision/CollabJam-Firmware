#include <Arduino.h>
#include <ble_esp.h>
#include <ble_services.h>
#include <vtproto.h>
#include <vtproto_handler/immediate_output.h>
// #include <vtproto_handler/tacton_output.h>
#include <tacton/tacton_store.h>
#include <vtproto_handler/tacton_receiver.h>

#include "SPIFFS.h"
#include "ble_pb_callback.h"
#include "ble_playback_request_callback.h"
#include "config.h"
#include "esp32_vtproto_interface.h"
#include "playback_request_receiver.h"
#include "tactile_display.h"
#include "tactile_display.pb.h"
#include "tacton_player_esp.h"
#include "vtproto_file_source.h"

namespace {
tact::vtproto::TactonStore tacton_store;

OutputMode current_output_mode = OutputMode_VTPROTO_REALTIME;
ChannelConfig channel_configs[config::display::kNumOfOutputs] = {
    {1, MotorType::MotorType_ERM, false, 0},
    {2, MotorType::MotorType_ERM, false, 0},
    {3, MotorType::MotorType_ERM, false, 0},
    {4, MotorType::MotorType_ERM, false, 0},
    {5, MotorType::MotorType_ERM, false, 0},
    {6, MotorType::MotorType_ERM, false, 0},
    {7, MotorType::MotorType_ERM, false, 0},
    {8, MotorType::MotorType_ERM, false, 0},
};
}  // namespace
namespace tact {
namespace vtproto {
// uint8_t vt_message_buffer[config::ble::kVtprotoBufSize];

// Tacton tacton;
}  // namespace vtproto
namespace display {

// uint8_t config_change_buffer[config::ble::kConfigChangeBufSize];

tact::vtproto::encode::DisplayConfigEncoder display_config_encoder(
    current_output_mode, channel_configs, config::display::kNumOfOutputs);
OutputMode last_active_mode = current_output_mode;

tact::vtproto::encode::ConfigOptionsEncoder config_options_encoder(
    (OutputMode*)config::display::kAvailableOuputModes,
    config::display::kAvailableOutputModesLen,
    (MotorType*)config::display::kAvailableMotorTypes,
    config::display::kAvailableMotorTypesLen, false);
}  // namespace display
namespace ble {
tact::ble::ConfigOptionsCallbackHandler config_options_callback_handler(
    &tact::display::display_config_encoder,
    tact::display::config_options_encoder.getAvalilableConfigurationOptions());
}  // namespace ble

EspVtprotoHardwareInterface vtp_esp_interface(
    (uint8_t)config::display::kNumOfOutputs,
    (uint8_t*)config::display::kMotorPins);
tact::vtproto::TactonPlayerESP tacton_player_esp(vtp_esp_interface);

tact::vtproto::TactonReceiver tacton_receiver(tacton_store.getNewTacton());
tact::vtproto::ImmediateOutputMode immediate_output_mode(vtp_esp_interface);
// tact::vtproto::ReceiveAndPlayTactonMode rec_and_play(vtp_esp_interface);
// tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);
// tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&tacton_receiver);
tact::vtproto::VtprotoFileSource file_source;

tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);

tact::vtproto::encode::TactonFileInformationListEncoder stored_tactons_encoder;
tact::vtproto::PlaybackRequestReceiver playback_req_receiver;
tact::ble::ReceivePlaybackRequestCallback ble_pb_req(
    (tact::vtproto::FileRequestReceiver*)&playback_req_receiver);
}  // namespace tact

void setup() {
#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  while (!Serial) {
  }
#endif

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  int x = 0;
  while (file) {
    tact::vtproto::TactonHeader th;
    th.file_header_ =
        tact::vtproto::decode::getHeader(th.pattern_name_, th.author_);
    tact::file_source.getTactonHeader((char*)file.name(), th);
    tact::vtproto::encode::TactonFileInformationEncode_t t;

    strncpy(t.filename_, file.name(), strlen(file.name()));
    t.filename_length_ = strlen(file.name());
    /*TODO Check if strlen is larger then max_len
        - throw error, because filename is accessor for file
        - alternatively truncate filename and introduce a accessor id into the
       TactonFileInformationList message
     */

    strncpy(t.author_, &th.author_[2], th.author_[1]);
    strncpy(t.pattern_name_, &th.pattern_name_[2], th.pattern_name_[1]);
    t.author_length_ = th.author_[1];
    t.pattern_name_length_ = th.author_[1];
    t.max_length_ = STORED_TACTONS_HEADER_MAX_STRLEN;
    tact::stored_tactons_encoder.addTactonFileInformation(
        t, th.file_header_.duration_ms, th.file_header_.n_instructions,
        th.file_header_.n_channels);
    ++x;
    file = root.openNextFile();
  }
  // Encode Message in characteristic
  //   tact::file_source.loadTactonFromFile("/test.tacton",
  //   &tact::tacton_receiver);

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
  characteristics[tact::ble::service_tactile_display::kChrNumberOfOutputs]
      ->setValue((uint8_t*)&config::display::kNumOfOutputs, 1);

  characteristics[tact::ble::service_tactile_display::kChrDisplayPlaybackState]
      ->setValue("IDLE");

  // characteristics[tact::ble::service_tactile_display::kChrDisplayPlaybackRequestStop]->setValue((uint16_t)0);
  // // No need to write?
  // TODO Check if value of constant is equal to propagated value

  characteristics
      [tact::ble::service_tactile_display::kChrModeVtprotoMaxMsgLength]
          ->setValue((uint16_t&)config::ble::kVtprotoBufSize);

  // Display config characteristic
  characteristics[tact::ble::service_tactile_display::kChrDisplayConfig]
      ->setValue(
          tact::display::display_config_encoder.getEncodedMessage(),
          tact::display::display_config_encoder.getEncodedMessageLength());

  characteristics[tact::ble::service_tactile_display::
                      kChrStoredTactonPlaybackRequestMaxLength]
      ->setValue((uint16_t&)config::ble::kReceivePlaybackRequestBufSize);

  // Available display config options buffer
  // Available channel configs buffer
  characteristics
      [tact::ble::service_tactile_display::kChrConfigAvailableOptions]
          ->setValue(
              tact::display::config_options_encoder.getEncodedMessage(),
              tact::display::config_options_encoder.getEncodedMessageLength());

  characteristics
      [tact::ble::service_tactile_display::kChrConfigChangeBufferMaxLength]
          ->setValue((uint16_t&)config::ble::kConfigChangeBufSize);

  characteristics[tact::ble::service_tactile_display::kChrStoredTactonsList]
      ->setValue(tact::stored_tactons_encoder.getEncodedMessage(),
                 tact::stored_tactons_encoder.getEncodedMessageLength());
  // Assign Callbacks to Characteristics
  characteristics[tact::ble::service_tactile_display::kChrModeVtprotoBuffer]
      ->setCallbacks(&tact::vtp_ble_callback);

  // Assign displayConfig characteristic to handler instance and instance to
  tact::ble::config_options_callback_handler.setBleCharacteristic(
      characteristics[tact::ble::service_tactile_display::kChrDisplayConfig]);
  characteristics[tact::ble::service_tactile_display::kChrConfigChangeBuffer]
      ->setCallbacks(&tact::ble::config_options_callback_handler);

  // Assign callback for file playback
  characteristics
      [tact::ble::service_tactile_display::kChrStoredTactonPlaybackRequest]
          ->setCallbacks(&tact::ble_pb_req);

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
    OutputMode new_mode =
        tact::display::display_config_encoder.getDisplayConfig()->output_mode;
    switch (new_mode) {
      case OutputMode_VTPROTO_REALTIME:
#ifdef DEBUG_SERIAL
        Serial.println("Switching to Realtime Mode");
#endif
        tact::display::last_active_mode = OutputMode_VTPROTO_REALTIME;
        tact::vtp_ble_callback.changeMessageReviecer(
            &tact::immediate_output_mode);
        break;
      case OutputMode_VTPROTO_TACTON:
#ifdef DEBUG_SERIAL
        Serial.println("Switching to Tacton Mode");
#endif
        tact::display::last_active_mode = OutputMode_VTPROTO_TACTON;
        tact::tacton_receiver.reset(tacton_store.getNewTacton());
        tact::vtp_ble_callback.changeMessageReviecer(&tact::tacton_receiver);
        // Reset TactonReceiver
        break;
      case OutputMode_VTPROTO_TACTON_HARDCODED:
#ifdef DEBUG_SERIAL
        Serial.println("Switching to Hardcoded Playback Mode");
#endif
        tact::display::last_active_mode = OutputMode_VTPROTO_TACTON_HARDCODED;
        tact::playback_req_receiver.reset();
        tact::tacton_receiver.reset(tacton_store.getNewTacton());
        // Reset TactonReceiver
        break;
      default:
        break;
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
      tact::tacton_player_esp.play(tacton_store.getLastTacton());
      tact::tacton_receiver.reset(tacton_store.getNewTacton());
      delay(1000);
      //   tact::file_source.loadTactonFromFile("/test.tacton",
      //                                        &tact::tacton_receiver);
    }

  } else if (tact::display::display_config_encoder.getDisplayConfig()
                 ->output_mode == OutputMode_VTPROTO_TACTON_HARDCODED) {
    if (tact::playback_req_receiver.receivedRequest()) {
#ifdef DEBUG_SERIAL
      Serial.println("Received a request!");
#endif
      tact::tacton_receiver.reset(tacton_store.getNewTacton());

      tact::file_source.loadTactonFromFile(
          (char*)tact::playback_req_receiver.getRequestedFilename().c_str(),
          &tact::tacton_receiver);
    }
    if (tact::tacton_receiver.hasReceivedTacton()) {
#ifdef DEBUG_SERIAL
      Serial.println("Loaded hardcoded tacton!");
#endif
      tact::tacton_player_esp.play(tacton_store.getLastTacton());
      tact::tacton_receiver.reset(tacton_store.getNewTacton());
    }
    // #ifdef DEBUG_SERIAL
    //     Serial.println("Waiting for new playback request!");
    // #endif
  } else {
  }
}
