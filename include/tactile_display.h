#ifndef _TACTILE_DISPLAY_H_
#define _TACTILE_DISPLAY_H_
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "tactile_display.pb.h"
// TODO Make all smart structs into classes
namespace tact {

namespace vtproto {
namespace encode {
const uint8_t kMaxStrLen = 64;
const uint8_t kMaxTacton = 32;
typedef struct ChannelConfigEncode_t {
  ChannelConfig* channel_configs_;
  uint32_t number_of_channels_;
} ChannelConfigEncode_t;

typedef struct DisplayConfigOptionsEncode_t {
  OutputMode* output_modes_;
  uint8_t number_of_modes_;
} DisplayConfigOptionsEncode_t;

typedef struct ChannelConfigOptionsEncode_t {
  MotorType* motor_types_;
  uint8_t number_of_types_;
  bool has_voltage;
} ChannelConfigOptionsEncode_t;

typedef struct TactonFileInformationEncode_t {
  // TactonFileInformation* tfi_;
  uint8_t max_length_ = kMaxStrLen;
  uint8_t filename_length_, author_length_, pattern_name_length_;
  char filename_[kMaxStrLen] = {0};
  char author_[kMaxStrLen] = {0};
  char pattern_name_[kMaxStrLen] = {0};
} TactonFileInformationEncode_t;

typedef struct TactonFileInformationListEncode_t {
  TactonFileInformation* tfi_;
  uint8_t number_of_files_;
} TactonFileInformationListEncode_t;

typedef struct DisplayConfigEncoder {
 private:
  uint8_t buffer_[255] = {0};
  uint8_t enc_msg_length_ = 0;
  OutputMode output_mode_;
  ChannelConfigEncode_t channel_configs_;
  DisplayConfig dc_;
  static bool encodeDisplayConfig(pb_ostream_t* stream,
                                  const pb_field_iter_t* field,
                                  void* const* arg);

 public:
  DisplayConfigEncoder(OutputMode output_mode, ChannelConfig* channel_configs,
                       uint32_t number_of_channels);
  uint8_t* getEncodedMessage();
  // uint8_t getBufferSize() { return sizeof(buffer_) / sizeof(buffer_[0]); }
  uint8_t getEncodedMessageLength() { return enc_msg_length_; }
  DisplayConfig* getDisplayConfig() { return &dc_; };
#ifdef DEBUG_SERIAL
  void print();
#endif  // DEBUG
} DisplayConfigEncoder;

typedef struct ConfigOptionsEncoder {
 private:
  uint8_t buffer_[64] = {0};
  uint8_t enc_msg_length_ = 0;
  DisplayConfigOptionsEncode_t dc_;
  ChannelConfigOptionsEncode_t cc_;
  AvailableConfigurationOptions aco_;
  static bool encodeChannelConfig(pb_ostream_t* stream,
                                  const pb_field_iter_t* field,
                                  void* const* arg);
  static bool encodeDisplayConfig(pb_ostream_t* stream,
                                  const pb_field_iter_t* field,
                                  void* const* arg);

 public:
  ConfigOptionsEncoder(OutputMode* output_modes, uint8_t number_of_modes,
                       MotorType* motor_types, uint8_t number_of_types,
                       bool has_voltage);
  uint8_t* getEncodedMessage();
  uint8_t getEncodedMessageLength() { return enc_msg_length_; };
  AvailableConfigurationOptions* getAvalilableConfigurationOptions() {
    return &aco_;
  }
} ConfigOptionsEncoder;

class TactonFileInformationListEncoder {
 public:
  TactonFileInformationListEncoder();
  uint8_t* getEncodedMessage();
  uint8_t getEncodedMessageLength() { return enc_msg_length_; }
  bool addTactonFileInformation(TactonFileInformationEncode_t tfi,
                                uint32_t duration_ms, uint32_t n_instructions,
                                uint32_t n_channels);

 private:
  uint8_t buffer_[512] = {0};
  uint8_t enc_msg_length_ = 0;
  uint8_t tfi_enc_len_ = 0;
  TactonFileInformationEncode_t tfi_encode_[kMaxTacton];
  TactonFileInformation tfi_[kMaxTacton];
  TactonFileInformationList tfil_;
  TactonFileInformationListEncode_t tfil_encode_;
  static bool encodeTactonFileInformationList(pb_ostream_t* stream,
                                              const pb_field_iter_t* field,
                                              void* const* arg);
  static bool encodeFilename(pb_ostream_t* stream, const pb_field_iter_t* field,
                             void* const* arg);
  static bool encodeAuthor(pb_ostream_t* stream, const pb_field_iter_t* field,
                           void* const* arg);
  static bool encodePatternName(pb_ostream_t* stream,
                                const pb_field_iter_t* field, void* const* arg);
};
}  // namespace encode

}  // namespace vtproto
namespace ble {
class ConfigOptionsCallbackHandler : public BLECharacteristicCallbacks {
 public:
  /*   ConfigOptionsCallbackHandler(
        tact::vtproto::encode::DisplayConfigEncoder* dce,
        AvailableConfigurationOptions* aoc,
        BLECharacteristic* display_config_ble_characteristic); */
  ConfigOptionsCallbackHandler(tact::vtproto::encode::DisplayConfigEncoder* dce,
                               AvailableConfigurationOptions* aoc);
  void setBleCharacteristic(
      BLECharacteristic* display_config_ble_characteristic) {
    dc_ble_characteristic_ = display_config_ble_characteristic;
  };
  void onWrite(BLECharacteristic* pCharacteristic);
  void onRead(BLECharacteristic* pCharacteristic);

 private:
  tact::vtproto::encode::DisplayConfigEncoder* dce_;
  AvailableConfigurationOptions* aoc_;
  BLECharacteristic* dc_ble_characteristic_;
};
}  // namespace ble
}  // namespace tact

#endif  // !_TACTILE_DISPLAY_H_