#ifndef ULN_PINOUTS_H_
#define ULN_PINOUTS_H_

// LIBRARY WIDE OVERRIDE VALUES
#define TACTILE_DISPLAY_NUM_OUTPUTS 5

namespace config {
namespace display {

const uint32_t kNumOfOutputs = TACTILE_DISPLAY_NUM_OUTPUTS;

#ifdef BOARD_CJ0
const uint8_t kMotorPins[kNumOfOutputs] = {13, 27, 26, 25, 33 /*  32*/};
#endif

#ifdef BOARD_CJ1
const uint8_t kMotorPins[kNumOfOutputs] = {13, 15, 4, 16,
                                           17 /* , 5, 18, 23} */};
#endif

}  // namespace display
}  // namespace config

#endif