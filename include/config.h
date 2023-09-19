#ifndef _TACT_FIRMWARE_CONFIG_H_
#define _TACT_FIRMWARE_CONFIG_H_

#include "tactile_display.pb.h"

// LIBRARY WIDE OVERRIDE VALUES
#define TACTILE_DISPLAY_NUM_OUTPUTS 5

namespace config {
namespace display {

// TODO: Move possible LRA/DRV configs in this file
const uint32_t kNumOfOutputs = TACTILE_DISPLAY_NUM_OUTPUTS;

#ifdef BOARD_RIA
const uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25,
                                           33 /*  32*/};
// // old navibration board
#endif

#ifdef BOARD_WHWS
const uint8_t kMotorPins[kNumOfOutputs] = {13, 15, 4, 16,
                                           17 /* , 5, 18, 23} */};  // whws
#endif
#ifdef BOARD_ZSOLT
const uint8_t kMotorPins[kNumOfOutputs] = {
    14, 27, 26, 25, 33 /* , 5, 18, 23} */};  // zsolts board
#endif
#ifdef BOARD_LNDW
const uint8_t kMotorPins[kNumOfOutputs] = {
    // 32, 26, 25, 33, 27 /* , 5, 18, 23} */};  // zsolts board
    32, 33, 25, 26, 27 /* , 5, 18, 23} */};  // zsolts board
#endif
const uint8_t kAvailableOutputModesLen = 3;
const uint8_t kAvailableMotorTypesLen = 3;

// DisplayConfig
const OutputMode kAvailableOuputModes[kAvailableOutputModesLen] = {
    OutputMode_VTPROTO_TACTON, OutputMode_VTPROTO_REALTIME,
    OutputMode_VTPROTO_TACTON_HARDCODED};
const MotorType kAvailableMotorTypes[kAvailableMotorTypesLen] = {
    MotorType_ERM, MotorType_PNEUMATIC};

}  // namespace display
namespace ble {
const std::string kDeviceName = "TB_TEST";
const uint16_t kVtprotoBufSize = 2048;
const uint16_t kConfigChangeBufSize = 255;
const uint16_t kReceivePlaybackRequestBufSize = 35;
}  // namespace ble
}  // namespace config

// config::ble::kDeviceName
// config::ble::kVtprotoBufSize
// config::ble::kConfigChangeBufSize

#endif