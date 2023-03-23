#ifndef TACTON_PLAYER_ESP_H_
#define TACTON_PLAYER_ESP_H_
// #include <interface/hardware_interface.h>
// #include <tacton/tacton.h>
// #include <vtproto.pb.h>

// namespace tact {

// namespace vtproto {

// struct TactonPlaybackTimes {
//   long t_0_ = 0;
//   long p_0_ = 0;
//   long t_elapsed_ = 0;
//   long p_elapsed_ = 0;
// };

// class TactonPlayerESP {
//  public:
//   TactonPlayerESP(tact::vtproto::HardwareInterface& output);
//   void play(Tacton* t);
//   tact::vtproto::HardwareInterface& output_;

//  private:
//   static void startThread(void* pvParameters);
//   TaskHandle_t task_;
//   TactonPlaybackTimes timing_;
// };
// }  // namespace vtproto

// }  // namespace tact

// #endif  // !TACTON_PLAYER_ESP_H_