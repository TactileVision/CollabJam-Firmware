// #include "vtproto_file_source.h"

// namespace tact {
// namespace vtproto {

// VtprotoFileSource::VtprotoFileSource() {
//   //   if (!SPIFFS.begin(true)) {
//   // #ifdef DEBUG
//   //     Serial.println("An Error has occurred while mounting SPIFFS");
//   // #endif
//   //     init_ = false;
//   //     // error_ = true;
//   //     return;
//   //   } else {
//   //     init_ = true;
//   //   }
// }
// VtprotoFileSource::~VtprotoFileSource() {}
// bool VtprotoFileSource::init() {
//   if (!SPIFFS.begin(true)) {
// #ifdef DEBUG
//     Serial.println("An Error has occurred while mounting SPIFFS");
// #endif
//     init_ = false;
//     // error_ = true;
//   } else {
//     init_ = true;
//   }
//   return init_;
// }
// void VtprotoFileSource::loadTactonFromFile(
//     char* filename, tact::vtproto::MessageReceiver* messageReceiver) {
//   if (!init_) {
//     if (!init()) return;
//   }

//   File file = SPIFFS.open(filename);
//   if (!file) {
// #ifdef DEBUG
//     Serial.println("Failed to open file for reading");
// #endif  // DEBUG
//     return;
//   }

//   file.readBytes(buf_, file.size());
//   pb_istream_t f_pb = pb_istream_from_buffer((pb_byte_t*)buf_, file.size());
//   file.close();

//   tact::vtproto::TactonHeader th;
//   th.file_header_ =
//       tact::vtproto::decode::getHeader(th.pattern_name_, th.author_);
//   pb_decode_ex(&f_pb, FileHeader_fields, &th.file_header_, PB_DECODE_DELIMITED);

//   messageReceiver->onHeader(th);

//   for (size_t i = 0; i < th.file_header_.n_instructions; i++) {
//     tact::vtproto::Group g;
//     Instruction instr = tact::vtproto::decode::getInstruction(g);
//     pb_decode_ex(&f_pb, Instruction_fields, &instr, PB_DECODE_DELIMITED);
//     messageReceiver->onInstruction(instr);
//   }
// }

// bool VtprotoFileSource::getTactonHeader(char* filename,
//                                         tact::vtproto::TactonHeader& dest) {
//   if (!init_) {
//     if (!init()) {
//       return false;
//     }
//   }

//   File file = SPIFFS.open(filename);
//   if (!file) {
// #ifdef DEBUG
//     Serial.println("Failed to open file for reading");
// #endif  // DEBUG
//     return false;
//   }

//   file.readBytes(buf_, file.size());
//   pb_istream_t f_pb = pb_istream_from_buffer((pb_byte_t*)buf_, file.size());
//   file.close();

//   // tact::vtproto::TactonHeader th;
//   // th.file_header_ =
//   //     tact::vtproto::decode::getHeader(th.pattern_name_, th.author_);
//   pb_decode_ex(&f_pb, FileHeader_fields, &dest.file_header_,
//                PB_DECODE_DELIMITED);

//   // return th;
//   // memcpy(dest.filename_, filename, kMaxTextLength);
//   // dest.filename_length_ = kMaxTextLength;

//   // memcpy(dest.author_, th.author_, kMaxTextLength);
//   // memcpy(dest.pattern_name_, th.pattern_name_, kMaxTextLength);
//   // dest.author_length_ = kMaxTextLength;
//   // dest.pattern_name_length_ = kMaxTextLength;

//   // dest.max_length_ = kMaxTextLength;
//   return true;
// }
// }  // namespace vtproto

// }  // namespace tact