#ifndef _TACT_FIRMWARE_CONFIG_H_
#define _TACT_FIRMWARE_CONFIG_H_

#include "tactile_display.pb.h"

// LIBRARY WIDE OVERRIDE VALUES
#define TACTILE_DISPLAY_NUM_OUTPUTS 3

namespace config {
namespace display {

const uint16_t kNumOfOutputs = TACTILE_DISPLAY_NUM_OUTPUTS;
const uint8_t kMotorPins[kNumOfOutputs] = {0, 1, 2};
// const uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25, 33, 32, 19, 21};

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
const std::string kDeviceName = "Tactile Display";
const uint16_t kVtprotoBufSize = 2048;
const uint16_t kConfigChangeBufSize = 255;
const uint16_t kReceivePlaybackRequestBufSize = 35;
}  // namespace ble
}  // namespace config

// config::ble::kDeviceName
// config::ble::kVtprotoBufSize
// config::ble::kConfigChangeBufSize

#endif