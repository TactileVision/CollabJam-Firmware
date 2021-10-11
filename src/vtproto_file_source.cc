#include "vtproto_file_source.h"

namespace tact {
namespace vtproto {

VtprotoFileSource::VtprotoFileSource() {
  //   if (!SPIFFS.begin(true)) {
  // #ifdef DEBUG_SERIAL
  //     Serial.println("An Error has occurred while mounting SPIFFS");
  // #endif
  //     init_ = false;
  //     // error_ = true;
  //     return;
  //   } else {
  //     init_ = true;
  //   }
}
VtprotoFileSource::~VtprotoFileSource() {}
void VtprotoFileSource::loadTactonFromFile(
    char* filename, tact::vtproto::MessageReceiver* messageReceiver) {
  if (!SPIFFS.begin(true)) {
#ifdef DEBUG_SERIAL
    Serial.println("An Error has occurred while mounting SPIFFS");
#endif
    init_ = false;
    // error_ = true;
    return;
  } else {
    init_ = true;
  }

#ifdef DEBUG_SERIAL
  Serial.println(init_);
#endif  // DEBUG
  File file = SPIFFS.open(filename);
  if (!file) {
#ifdef DEBUG_SERIAL
    Serial.println("Failed to open file for reading");
#endif  // DEBUG
        // error_ = true;
    return;
  }

  // buffer  reset?
  // #ifdef DEBUG_SERIAL
  //   Serial.println(file.size());
  // #endif  // DEBUG
  file.readBytes(buf_, file.size());
  pb_istream_t f_pb = pb_istream_from_buffer((pb_byte_t*)buf_, file.size());
  file.close();

  tact::vtproto::TactonHeader th;

  th.file_header_ =
      tact::vtproto::decode::getHeader(th.pattern_name_, th.author_);
  pb_decode_ex(&f_pb, FileHeader_fields, &th.file_header_, PB_DECODE_DELIMITED);

  messageReceiver->onHeader(th);

  for (size_t i = 0; i < th.file_header_.n_instructions; i++) {
    tact::vtproto::Group g;
    Instruction instr = tact::vtproto::decode::getInstruction(g);
    pb_decode_ex(&f_pb, Instruction_fields, &instr, PB_DECODE_DELIMITED);
    messageReceiver->onInstruction(instr);
  }
}

}  // namespace vtproto

}  // namespace tact