#ifndef _VTPRPTO_FILE_SOURCE_H_
#define _VTPRPTO_FILE_SOURCE_H_
#include <Arduino.h>
#include <interface/message_interface.h>

#include "SPIFFS.h"
#include "pb_decode.h"
#include "tactile_display.h"
#include "tacton.h"
/*
Store binary tacton files in the /data folder and upload them to the esp via
project tasks: Platform > Build Filesystem Image
*/
namespace tact {
namespace vtproto {
const uint16_t kMaxBufferSize = 256;
class VtprotoFileSource {
 private:
  bool init_ = false;
  char buf_[kMaxBufferSize];
  bool init();

 public:
  VtprotoFileSource();
  ~VtprotoFileSource();
  void loadTactonFromFile(char* filename,
                          tact::vtproto::MessageReceiver* messageReceiver);
  bool getTactonHeader(char* filename, tact::vtproto::TactonHeader& dest);
};
}  // namespace vtproto
}  // namespace tact

#endif  // !_VTPRPTO_FILE_SOURCE_H_