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

  //   getDisplayConfig();
};

DisplayConfig DisplayConfigEncoder::getDisplayConfig() { return dc_; }

bool DisplayConfigEncoder::encodeDisplayConfig(pb_ostream_t* stream,
                                               const pb_field_iter_t* field,
                                               void* const* arg) {
  ChannelConfigEncode_t* c = (ChannelConfigEncode_t*)(*arg);
  Serial.println("ENCODING!");
  Serial.println(c->number_of_channels_);

  // Calculate size of message
  //   pb_ostream_t substream = PB_OSTREAM_SIZING;
  //   size_t size;
  //   for (uint32_t i = 0; i < c->number_of_channels_; i++) {
  //     if (!pb_encode(&substream, ChannelConfig_fields,
  //     &c->channel_configs_[i])) {
  // #ifdef DEBUG_SERIAL
  // #ifndef UNIT_TEST
  //       const char* error = PB_GET_ERROR(stream);
  //       Serial.print("Channel config encoding error: ");
  //       Serial.println(error);
  // #endif  // UNIT_TEST
  // #endif  // DEBUG
  //       return false;
  //     };
  //   }
  //   size = substream.bytes_written;
  // #ifdef DEBUG_SERIAL
  // #ifndef UNIT_TEST
  //   Serial.print("substream bytes written: ");
  //   Serial.println(size);
  //   Serial.print("encoding ids for field tag: ");
  //   Serial.println(field->tag);
  // #endif  // UNIT_TEST
  // #endif  // DEBUG

  //   if (!pb_encode_tag(stream, PB_WT_STRING, field->tag)) {
  // #ifdef DEBUG_SERIAL
  // #ifndef UNIT_TEST
  //     const char* error = PB_GET_ERROR(stream);
  //     Serial.print("output id encoding error: ");
  //     Serial.println(error);
  // #endif  // UNIT_TEST
  // #endif  // DEBUG
  //     return false;
  //   }
  //   if (!pb_encode_varint(stream, size)) {
  // #ifdef DEBUG_SERIAL
  // #ifndef UNIT_TEST
  //     const char* error = PB_GET_ERROR(stream);
  //     // Serial.printf("output id encoding error: %s\n", error);
  //     Serial.print("output id encoding error: ");
  //     Serial.println(error);
  // #endif  // UNIT_TEST
  // #endif  // DEBUG
  //     return false;
  //   }

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
#ifdef DEBUG_SERIAL
#ifndef UNIT_TEST
    Serial.print("Channel number: ");
    Serial.println(c->channel_configs_[i].channel_number);
    Serial.print("Motor type: ");
    Serial.println(c->channel_configs_[i].motor_type);
    Serial.print("Has voltage: ");
    Serial.println(c->channel_configs_[i].has_voltage);
    Serial.print("Voltage: ");
    Serial.println(c->channel_configs_[i].voltage);
    Serial.println();
#endif  // UNIT_TEST
#endif  // DEBUG
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

}  // namespace encode

}  // namespace vtproto

}  // namespace tact
