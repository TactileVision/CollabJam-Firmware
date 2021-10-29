#include <Arduino.h>

#include "SPIFFS.h"
#include "ble_esp.h"
#include "ble_pb_callback.h"
#include "ble_playback_request_callback.h"
#include "ble_services.h"
#include "config.h"
#include "tacton/tacton.h"
#include "tacton/tacton_store.h"
#include "vtproto.h"
#include "vtproto_handler/immediate_output.h"
#include "vtproto_handler/tacton_receiver.h"
// #include "hardware_interfaces/esp32_hw_interface.h"
#include "hardware_interfaces/i2c_mp_drv_hw_interface.h"
#include "playback_request_receiver.h"
#include "tactile_display.h"
#include "tactile_display.pb.h"
#include "tacton_player_esp.h"
#include "vtproto_file_source.h"

namespace {
tact::vtproto::TactonStore tacton_store;

// See resources/tacile_display.vtproto
// BLE tactile display config
OutputMode current_output_mode = OutputMode_VTPROTO_REALTIME;
OutputMode last_active_mode = current_output_mode;
ChannelConfig channel_configs[config::display::kNumOfOutputs] = {
    {1, MotorType::MotorType_ERM, false, 0},
    {2, MotorType::MotorType_ERM, false, 0},
    {3, MotorType::MotorType_ERM, false, 0},
    // {4, MotorType::MotorType_ERM, false, 0},
    // {5, MotorType::MotorType_ERM, false, 0},
    // {6, MotorType::MotorType_ERM, false, 0},
    // {7, MotorType::MotorType_ERM, false, 0},
    // {8, MotorType::MotorType_ERM, false, 0},
};

// BLE tactile display config -  protobuf encoder
tact::vtproto::encode::DisplayConfigEncoder display_config_encoder(
    current_output_mode, channel_configs, config::display::kNumOfOutputs);
tact::vtproto::encode::ConfigOptionsEncoder config_options_encoder(
    (OutputMode*)config::display::kAvailableOuputModes,
    config::display::kAvailableOutputModesLen,
    (MotorType*)config::display::kAvailableMotorTypes,
    config::display::kAvailableMotorTypesLen, false);
tact::vtproto::encode::TactonFileInformationListEncoder stored_tactons_encoder;

tact::ble::ConfigOptionsCallbackHandler config_options_callback_handler(
    &display_config_encoder,
    config_options_encoder.getAvalilableConfigurationOptions());

// Hardware Interfaces
tact::I2CMultiplexerDrvInterface vtp_esp_interface(
    (uint8_t)config::display::kNumOfOutputs,
    (uint8_t*)config::display::kMotorPins);

// GPIO with ULN2803A
// EspVtprotoHardwareInterface vtp_esp_interface(
//     (uint8_t)config::display::kNumOfOutputs,
//     (uint8_t*)config::display::kMotorPins);

// vptoro tacton file (bytes) handling
tact::vtproto::VtprotoFileSource file_source;

// BLE vtproto receiver (messageReceiver)
tact::vtproto::TactonReceiver tacton_receiver(tacton_store.getNewTacton());
tact::vtproto::ImmediateOutputMode immediate_output_mode(vtp_esp_interface);

// vtptoto tacton player
tact::vtproto::TactonPlayerESP tacton_player_esp(vtp_esp_interface);

// Will be called when vtproto BLE buffer is written, paseses header or
// instruction to a messageReceiver
tact::ble::ReceiveVtprotoCallback vtp_ble_callback(&immediate_output_mode);

// Shows if a file playback was requested and provides the requested filename
tact::vtproto::PlaybackRequestReceiver playback_req_receiver;

// Will be called when Stored_Tactons_Request_Playback buffer is written
tact::ble::ReceivePlaybackRequestCallback ble_pb_req(
    (tact::vtproto::FileRequestReceiver*)&playback_req_receiver);

}  // namespace

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) {
  }
#endif
  /*   Iterate trough all files and move file information into
    TactonFileInformationListEncoder  */
  // TODO Encapsulate into seperate function/class
  // TODO Simplify copy process
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
    file_source.getTactonHeader((char*)file.name(), th);
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
    stored_tactons_encoder.addTactonFileInformation(
        t, th.file_header_.duration_ms, th.file_header_.n_instructions,
        th.file_header_.n_channels);
    ++x;
    file = root.openNextFile();
  }

  /* Init TCA/DRV hardware. Cannot be moved into constructor, because i2c
    operations have to occur inside setup/loop (?) */
  vtp_esp_interface.init();

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

  // Not used yet
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
      ->setValue(display_config_encoder.getEncodedMessage(),
                 display_config_encoder.getEncodedMessageLength());

  characteristics[tact::ble::service_tactile_display::
                      kChrStoredTactonPlaybackRequestMaxLength]
      ->setValue((uint16_t&)config::ble::kReceivePlaybackRequestBufSize);

  // Available display config options buffer
  // Available channel config options buffer
  characteristics
      [tact::ble::service_tactile_display::kChrConfigAvailableOptions]
          ->setValue(config_options_encoder.getEncodedMessage(),
                     config_options_encoder.getEncodedMessageLength());

  characteristics
      [tact::ble::service_tactile_display::kChrConfigChangeBufferMaxLength]
          ->setValue((uint16_t&)config::ble::kConfigChangeBufSize);

  // Stored tactons
  characteristics[tact::ble::service_tactile_display::kChrStoredTactonsList]
      ->setValue(stored_tactons_encoder.getEncodedMessage(),
                 stored_tactons_encoder.getEncodedMessageLength());

  // Assign Callbacks to Characteristics
  characteristics[tact::ble::service_tactile_display::kChrModeVtprotoBuffer]
      ->setCallbacks(&vtp_ble_callback);

  // Assign displayConfig ble characterstic to displayConfigChangeHandler
  config_options_callback_handler.setBleCharacteristic(
      characteristics[tact::ble::service_tactile_display::kChrDisplayConfig]);
  characteristics[tact::ble::service_tactile_display::kChrConfigChangeBuffer]
      ->setCallbacks(&config_options_callback_handler);

  // Assign callback for file playback
  characteristics
      [tact::ble::service_tactile_display::kChrStoredTactonPlaybackRequest]
          ->setCallbacks(&ble_pb_req);

  service->start();

  BLEDevice::startAdvertising();

#ifdef DEBUG
  Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}

void loop() {
  // Check current mode
  if (last_active_mode !=
      display_config_encoder.getDisplayConfig()->output_mode) {
    OutputMode new_mode =
        display_config_encoder.getDisplayConfig()->output_mode;
    switch (new_mode) {
      case OutputMode_VTPROTO_REALTIME:
#ifdef DEBUG
        Serial.println("Switching to Realtime Mode");
#endif
        last_active_mode = OutputMode_VTPROTO_REALTIME;
        vtp_ble_callback.changeMessageReviecer(&immediate_output_mode);
        break;
      case OutputMode_VTPROTO_TACTON:
#ifdef DEBUG
        Serial.println("Switching to Tacton Mode");
#endif
        last_active_mode = OutputMode_VTPROTO_TACTON;
        tacton_receiver.reset(tacton_store.getNewTacton());
        vtp_ble_callback.changeMessageReviecer(&tacton_receiver);
        // Reset TactonReceiver
        break;
      case OutputMode_VTPROTO_TACTON_HARDCODED:
#ifdef DEBUG
        Serial.println("Switching to Hardcoded Playback Mode");
#endif
        last_active_mode = OutputMode_VTPROTO_TACTON_HARDCODED;
        playback_req_receiver.reset();
        tacton_receiver.reset(tacton_store.getNewTacton());
        // Reset TactonReceiver
        break;
      default:
        break;
    }
  }

  if (display_config_encoder.getDisplayConfig()->output_mode ==
      OutputMode_VTPROTO_TACTON) {
    // TODO Check if device is playing or not
    if (tacton_receiver.hasReceivedTacton()) {
#ifdef DEBUG
      Serial.println("Received a tacton!");
#endif
      // Play back tacton
      tacton_player_esp.play(tacton_store.getLastTacton());
      tacton_receiver.reset(tacton_store.getNewTacton());
#ifdef DEBUG
      Serial.println("Tacton playback finished!");
#endif
    }

  } else if (display_config_encoder.getDisplayConfig()->output_mode ==
             OutputMode_VTPROTO_TACTON_HARDCODED) {
    if (playback_req_receiver.receivedRequest()) {
#ifdef DEBUG
      Serial.println("Received a request!");
#endif
      tacton_receiver.reset(tacton_store.getNewTacton());

      file_source.loadTactonFromFile(
          (char*)playback_req_receiver.getRequestedFilename().c_str(),
          &tacton_receiver);
    }
    if (tacton_receiver.hasReceivedTacton()) {
#ifdef DEBUG
      Serial.println("Loaded hardcoded tacton!");
#endif
      tacton_player_esp.play(tacton_store.getLastTacton());
      tacton_receiver.reset(tacton_store.getNewTacton());
    }
  } else {
  }
}
