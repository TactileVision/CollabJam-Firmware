#ifndef _PLAYBACK_REQUEST_RECEIVER_H_
#define _PLAYBACK_REQUEST_RECEIVER_H_
#include <Arduino.h>
#include <interface/file_interface.h>

#include "vtproto_file_source.h"
namespace tact {
namespace vtproto {
class PlaybackRequestReceiver : FileRequestReceiver {
 private:
  bool received_request_ = false;
  // VtprotoFileSource* file_source_;
  std::string requested_filename_;

 public:
  PlaybackRequestReceiver();
  ~PlaybackRequestReceiver();
  void onFilename(std::string filename);
  bool receivedRequest() { return received_request_; }
  std::string getRequestedFilename();
  void reset() { received_request_ = false; };
};
}  // namespace vtproto

}  // namespace tact
#endif  // !_PLAYBACK_REQUEST_RECEIVER_H_