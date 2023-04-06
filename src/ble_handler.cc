#include "ble_handler.h"

namespace tact {
namespace ble {

BLEVibrationHandler::BLEVibrationHandler(tact::vtproto::HardwareInterface &h)
    : output_(h),
      instruction_queue_(cppQueue(sizeof(DisplayUpdate), 32, FIFO, true)) {}

void BLEVibrationHandler::onWrite(BLECharacteristic *pCharacteristic) {
  /*
  Read Data from buffer
  Decode data with pb_decode_ex, only expect instructions
  execute instructions after receivicing them
  */
#ifdef DEBUG
  Serial.printf("Received message with length %i\n",
                pCharacteristic->getValue().length());
  // for (size_t i = 0; i < pCharacteristic->getValue().length(); i++) {
  //   Serial.printf("%X ", pCharacteristic->getData()[i]);
  // }
  // Serial.println();
#endif
  DisplayUpdate update;
  uint8_t *data = pCharacteristic->getData();
  uint8_t len = pCharacteristic->getValue().length() > 5
                    ? 5
                    : pCharacteristic->getValue().length();
  for (size_t i = 0; i < len; i++) {
    Serial.printf("Value for channel %i: %i\n", i, data[i]);
    update.values[i] = data[i];
  }

  instruction_queue_.push(&update);
#ifdef DEBUG
  Serial.printf("Execution done");
#endif
}

void BLEVibrationHandler::onRead(BLECharacteristic *pCharacteristic) {}

void BLEVibrationHandler::updateDisplay(uint8_t *values, uint8_t length) {
  // TODO Update channels with the received updates
  for (size_t i = 0; i < length; i++) {
    // 255 is used as a symbol with the meaning "Don't change this channel"
    if (values[i] != 255) {
      output_.setIntensity(i, values[i]);
    }
  }
}

// TODO: Make distinctions between instructions more clear!
// void VtprotoHandler::setInstantParameter(
//     InstInstantlySetParameter instruction) {
//   if (instruction.which_parameter ==
//       InstInstantlySetParameter_frequency_hz_tag) {
// #ifdef DEBUG
//     Serial.println("Setting frequency value is not supported by this
//     device");
// #endif  // DEBUG_TACTILE

//     // output_.setFrequency(instruction.target_id.channel_id,
//     //                     instruction.parameter.frequency_hz);
//   }
//   if (instruction.which_target_id ==
//   InstInstantlySetParameter_channel_id_tag) {
// #ifdef DEBUG
//     Serial.println("Setting channel value");
// #endif
//     received_state_.setPinValue(instruction.target_id.channel_id,
//                                 instruction.parameter.intensity);

//     // TODO: Refactor into own class and neat funtion
//     // Compensates the pwm output values based on the number of active
//     outputs for (uint8_t i = 0; i < TACTILE_DISPLAY_NUM_OUTPUTS; i++) {
//       compensated_state_.setPinValue(
//           i, received_state_.getPinValue(i) == 0
//                  ? 0
//                  : tact::vtproto::getCompensatedFloat(
//                        received_state_.getPinValue(i), COMPENSATION_NEW_MIN,
//                        COMPENSATION_NEW_MAX, COMPENSATION_OFFSET,
//                        received_state_.getNumberOfActivePins()));
//       output_.setIntensity(i, compensated_state_.getPinValue(i));
//     }
//   } else  // Groups
//   {
// #ifdef DEBUG
//     Serial.println("Setting group value");
// #endif
//     if (instruction.target_id.group_id == 0) {
//       for (uint8_t i = 0; i < output_.getActuatorCount(); i++) {
//         output_.setIntensity(i, instruction.parameter.intensity);
//       }
//     } else {
//       // Get Group from groups_ with received group id
//       // Loop through all channels in group
//       // Loop through all groups in group
//       tact::vtproto::Group *group =
//           group_manager_.getGroupByGroupId(instruction.target_id.group_id);
//       if (group == NULL) {
// #ifdef DEBUG
//         Serial.println("Group with the specified id not found");
// #endif
//         return;
//       }
//       for (uint16_t i = 1; i <= group->channels_.count(); i++) {
//         output_.setIntensity(group->channels_.getNth(i),
//                              instruction.parameter.intensity);
//       }

//       // TODO Define recursive way of calling groups from group xP

// #ifdef DEBUG
//       if (group->groups_.count() > 0)
//         Serial.println("Calling groups from groups not supported yet");
// #endif
//     }
//   }
// #ifdef DEBUG
//   Serial.println("Done setInstantParameter ");
// #endif
// }
}  // namespace ble
}  // namespace tact