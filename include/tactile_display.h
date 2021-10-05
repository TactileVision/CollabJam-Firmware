#ifndef _TACTILE_DISPLAY_H_
#define _TACTILE_DISPLAY_H_
#include <Arduino.h>
#include <pb_encode.h>

#include "tactile_display.pb.h"
namespace tact {

namespace vtproto {
namespace encode {
typedef struct ChannelConfigEncode_t {
  ChannelConfig* channel_configs_;
  uint32_t number_of_channels_;
} ChannelConfigEncode_t;

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
  DisplayConfig getDisplayConfig();
  uint8_t* getEncodedMessage();
  // uint8_t getBufferSize() { return sizeof(buffer_) / sizeof(buffer_[0]); }
  uint8_t getEncodedMessageLength() { return enc_msg_length_; }
} DisplayConfigEncoder;
// bool encodeDisplayConfig(pb_ostream_t* stream, const pb_field_iter_t* field,
//                          void* const* arg);
// DisplayConfig getDisplayConfig();
}  // namespace encode

}  // namespace vtproto

}  // namespace tact

#endif  // !_TACTILE_DISPLAY_H_