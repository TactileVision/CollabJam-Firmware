#ifndef ULN_PINOUTS_H_
#define ULN_PINOUTS_H_


// LIBRARY WIDE OVERRIDE VALUES
#define TACTILE_DISPLAY_NUM_OUTPUTS 5

namespace config {
namespace display {

const uint32_t kNumOfOutputs = TACTILE_DISPLAY_NUM_OUTPUTS;

#ifdef BOARD_RIA
const uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25, 33 /*  32*/};
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

}  // namespace display
}  // namespace config

#endif