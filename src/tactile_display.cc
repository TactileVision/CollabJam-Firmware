#include "tactile_display.h"

namespace tact {
namespace vtproto {
namespace encode {

DisplayConfigEncoder::DisplayConfigEncoder(OutputMode output_mode,
                                           ChannelConfig* channel_configs,
                                           uint32_t number_of_channels) {
  output_mode_ = output_mode;
  channel_configs_.channel_configs_ = channel_configs;
  channel_configs_.number_of_channels_ = number_of_channels;

  dc_ = DisplayConfig_init_default;
  dc_.output_mode = output_mode;
  dc_.number_of_channels = number_of_channels;
  dc_.channel_configs.arg = &channel_configs_;
  dc_.channel_configs.funcs.encode = DisplayConfigEncoder::encodeDisplayConfig;
};

bool DisplayConfigEncoder::encodeDisplayConfig(pb_ostream_t* stream,
                                               const pb_field_iter_t* field,
                                               void* const* arg) {
  ChannelConfigEncode_t* c = (ChannelConfigEncode_t*)(*arg);
  // Serial.println("ENCODING!");
  // Serial.println(c->number_of_channels_);

  for (uint32_t i = 0; i < c->number_of_channels_; i++) {
    if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      Serial.print("output id encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    }
    if (!pb_encode_submessage(stream, ChannelConfig_fields,
                              &c->channel_configs_[i])) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      Serial.print("Channel config encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    };
  }
  return true;
};

uint8_t* DisplayConfigEncoder::getEncodedMessage() {
  bool status;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer_, sizeof(buffer_));
  status = pb_encode(&stream, DisplayConfig_fields, &dc_);
  enc_msg_length_ = stream.bytes_written;

  if (!status) {
    Serial.print("Encoding failed: ");
    Serial.println(PB_GET_ERROR(&stream));
  } else {
    Serial.print(enc_msg_length_);
    Serial.println(" Bytes written");
    for (uint32_t i = 0; i < enc_msg_length_; i++) {
      Serial.print(buffer_[i]);
    }
    Serial.println();
  }
  return buffer_;
}
#ifdef DEBUG_SERIAL
void DisplayConfigEncoder::print() {
  Serial.print("Encoder stored output mode: ");
  Serial.println(output_mode_);

  Serial.print("Message length: ");
  Serial.println(enc_msg_length_);

  Serial.println("Display config");
  Serial.print("Output Mode: ");
  Serial.println(dc_.output_mode);
  Serial.print("Number of channels:");
  Serial.println(dc_.number_of_channels);

  // ChannelConfigEncode_t* cc =
  //     (ChannelConfigEncode_t*)(*dc_.channel_configs.arg);
  // for (size_t i = 0; i < cc->number_of_channels_; i++) {
  //   Serial.print("Channel: ");
  //   Serial.println(cc[i]->channel_configs_->channel_number);
  //   Serial.print("Motor type: ");
  //   Serial.println(cc[i]->channel_configs_->motor_type);
  //   Serial.print("Voltage: ");
  //   Serial.println(cc[i]->channel_configs_->has_voltage);
  //   if (cc[i]->channel_configs_->has_voltage){

  //     Serial.println(cc[i]->channel_configs_->voltage);
  //   }
  // }
}
#endif  // DEBUG
ConfigOptionsEncoder::ConfigOptionsEncoder(OutputMode* output_modes,
                                           uint8_t number_of_modes,
                                           MotorType* motor_types,
                                           uint8_t number_of_types,
                                           bool has_voltage) {
  dc_.output_modes_ = output_modes;
  dc_.number_of_modes_ = number_of_modes;
  cc_.motor_types_ = motor_types;
  cc_.number_of_types_ = number_of_types;
  cc_.has_voltage = has_voltage;
  aco_ = AvailableConfigurationOptions_init_default;
  aco_.display_config_options = AvailableDisplayConfigOptions_init_default;
  aco_.display_config_options.output_modes.arg = &dc_;
  aco_.display_config_options.output_modes.funcs.encode =
      ConfigOptionsEncoder::encodeDisplayConfig;
  aco_.has_display_config_options = true;

  aco_.channel_config_options = AvailableChannelConfigOptions_init_default;
  aco_.channel_config_options.variable_voltage = cc_.has_voltage;
  aco_.channel_config_options.motor_types.arg = &cc_;
  aco_.channel_config_options.motor_types.funcs.encode =
      ConfigOptionsEncoder::encodeChannelConfig;
  aco_.has_channel_config_options = true;
}

bool ConfigOptionsEncoder::encodeChannelConfig(pb_ostream_t* stream,
                                               const pb_field_iter_t* field,
                                               void* const* arg) {
  ChannelConfigOptionsEncode_t* c = (ChannelConfigOptionsEncode_t*)(*arg);
  Serial.print("Encoding channel config options ");
  Serial.print("number of types: ");
  Serial.println(c->number_of_types_);
  // Packed encoding!!!
  // calc size via writing to substream
  pb_ostream_t substream = PB_OSTREAM_SIZING;
  size_t size;
  for (uint8_t i = 0; i < c->number_of_types_; i++) {
    if (!pb_encode_varint(&substream, (uint64_t)c->motor_types_[i])) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      // Serial.printf("output id encoding error: %s\n", error);
      Serial.print("output id encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    }
  }
  size = substream.bytes_written;
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
  // Serial.printf("substream bytes written: %i\n", size);
  Serial.print("substream bytes written: ");
  Serial.println(size);
  // First elements contains length of the passed array
  // Serial.printf("encoding ids for field tag: %i\n", field->tag);
  Serial.print("encoding ids for field tag: ");
  Serial.println(field->tag);
#endif  // UNIT_TEST
#endif  // DEBUG

  // write tag
  if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
    const char* error = PB_GET_ERROR(stream);
    // Serial.printf("output id encoding error: %s\n", error);
    Serial.print("output id encoding error: ");
    Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
    return false;
  }

  // write size
  if (!pb_encode_varint(stream, size)) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
    const char* error = PB_GET_ERROR(stream);
    // Serial.printf("output id encoding error: %s\n", error);
    Serial.print("output id encoding error: ");
    Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
    return false;
  }
  // write data
  for (uint8_t i = 0; i < c->number_of_types_; i++) {
    if (!pb_encode_varint(stream, (uint64_t)c->motor_types_[i])) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      // Serial.printf("output id encoding error: %s\n", error);
      Serial.print("output id encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    }
  }
  return true;
};

bool ConfigOptionsEncoder::encodeDisplayConfig(pb_ostream_t* stream,
                                               const pb_field_iter_t* field,
                                               void* const* arg) {
  DisplayConfigOptionsEncode_t* d = (DisplayConfigOptionsEncode_t*)(*arg);
  Serial.print("Encoding display config options ");
  // Packed encoding!!!
  // calc size via writing to substream
  pb_ostream_t substream = PB_OSTREAM_SIZING;
  size_t size;
  for (uint8_t i = 0; i < d->number_of_modes_; i++) {
    if (!pb_encode_varint(&substream, (uint64_t)d->output_modes_[i])) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      // Serial.printf("output id encoding error: %s\n", error);
      Serial.print("output id encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    }
  }
  size = substream.bytes_written;
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
  // Serial.printf("substream bytes written: %i\n", size);
  Serial.print("substream bytes written: ");
  Serial.println(size);
  // First elements contains length of the passed array
  // Serial.printf("encoding ids for field tag: %i\n", field->tag);
  Serial.print("encoding ids for field tag: ");
  Serial.println(field->tag);
#endif  // UNIT_TEST
#endif  // DEBUG

  // write tag
  if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
    const char* error = PB_GET_ERROR(stream);
    // Serial.printf("output id encoding error: %s\n", error);
    Serial.print("output id encoding error: ");
    Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
    return false;
  }

  // write size
  if (!pb_encode_varint(stream, size)) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
    const char* error = PB_GET_ERROR(stream);
    // Serial.printf("output id encoding error: %s\n", error);
    Serial.print("output id encoding error: ");
    Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
    return false;
  }
  // write data
  for (uint8_t i = 0; i < d->number_of_modes_; i++) {
    if (!pb_encode_varint(stream, (uint64_t)d->output_modes_[i])) {
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
      const char* error = PB_GET_ERROR(stream);
      // Serial.printf("output id encoding error: %s\n", error);
      Serial.print("output id encoding error: ");
      Serial.println(error);
#endif  // UNIT_TEST
#endif  // DEBUG
      return false;
    }
  }
  return true;
};

uint8_t* ConfigOptionsEncoder::getEncodedMessage() {
  bool status;
  Serial.print("Encoding config options ");
  pb_ostream_t stream = pb_ostream_from_buffer(buffer_, sizeof(buffer_));
  status = pb_encode(&stream, AvailableConfigurationOptions_fields, &aco_);
  enc_msg_length_ = stream.bytes_written;

  if (!status) {
    Serial.print("Encoding failed: ");
    Serial.println(PB_GET_ERROR(&stream));
  } else {
    Serial.print(enc_msg_length_);
    Serial.println(" Bytes written");
    for (uint32_t i = 0; i < enc_msg_length_; i++) {
      Serial.print(buffer_[i]);
    }
    Serial.println();
  }
  return buffer_;
}
}  // namespace encode

}  // namespace vtproto
namespace ble {

// ConfigOptionsCallbackHandler::ConfigOptionsCallbackHandler(
//     tact::vtproto::encode::DisplayConfigEncoder* dce,
//     AvailableConfigurationOptions* aoc,
//     BLECharacteristic* display_config_ble_characteristic)
//     : dce_(dce),
//       aoc_(aoc),
//       dc_ble_characteristic_(display_config_ble_characteristic) {}

ConfigOptionsCallbackHandler::ConfigOptionsCallbackHandler(
    tact::vtproto::encode::DisplayConfigEncoder* dce,
    AvailableConfigurationOptions* aoc)
    : dce_(dce), aoc_(aoc) {}

void ConfigOptionsCallbackHandler::onWrite(BLECharacteristic* pCharacteristic) {
#ifdef DEBUG_SERIAL
  Serial.println("ConfigOptionsBuffer written ");
#endif  // DEBUG

  ReqSetDisplayConfig rsdc = ReqSetDisplayConfig_init_zero;
  pb_istream_t is = pb_istream_from_buffer(
      pCharacteristic->getData(), pCharacteristic->getValue().length());
  if (!pb_decode(&is, ReqSetDisplayConfig_fields, &rsdc)) {
#ifdef DEBUG_SERIAL
    Serial.println("Decoding received ReqSetDisplayConfig message failed");
#endif  // DEBUG
  }

  // TODO Check if received options are available for display
  dc_ble_characteristic_->setValue(dce_->getEncodedMessage(),
                                   dce_->getEncodedMessageLength());
  dc_ble_characteristic_->notify();
}
void ConfigOptionsCallbackHandler::onRead(BLECharacteristic* pCharacteristic) {
#ifdef DEBUG_SERIAL
  Serial.print("ConfigOptionsBuffer read: ");
  Serial.println(pCharacteristic->getValue().c_str());
}
#endif  // DEBUG
}  // namespace ble
}  // namespace tact
