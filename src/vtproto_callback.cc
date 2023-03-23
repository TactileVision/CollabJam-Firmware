#include "vtproto_callback.h"

namespace tact {
namespace ble {

VtprotoHandler::VtprotoHandler(tact::vtproto::HardwareInterface &h)
    : output_(h),
      instruction_queue_(
          cppQueue(sizeof(InstructionQueueItem), 32, FIFO, true)) {}

// void VtprotoHandler::attachHardwareInterface(
//     tact::vtproto::HardwareInterface &h) {
//   this->output_ = h;
// }

void VtprotoHandler::onWrite(BLECharacteristic *pCharacteristic) {
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

  // Read the ble data as an nanopb input stream
  pb_istream_t istream_ = pb_istream_from_buffer(
      pCharacteristic->getData(), pCharacteristic->getValue().length());

  while (istream_.bytes_left > 0) {
// We only expect to receive instructions, therefore we will immediately
// start to parse those ignoring headers. The sensing application has to be
// aware of that
#ifdef DEBUG
    Serial.println("Decoding instruction");
#endif
    // BUG: Move Instruction into a permanent spot so it won't be deletd after
    // calling the function
    InstructionQueueItem i;
    i.instruction = Instruction_init_default;
    // instruction_ = Instruction_init_default;
    // tact::vtproto::Group g;  // vtproto group

    // Set the decoding funtion and its arguments to our custom function and
    // group object
    i.instruction.cb_concrete_instruction.funcs.decode =
        tact::vtproto::decode::callbackInstruction;
    i.instruction.cb_concrete_instruction.arg = &i.group;

    // parse the received instruction
    if (!pb_decode_ex(&istream_, Instruction_fields, &i.instruction,
                      PB_DECODE_DELIMITED | PB_DECODE_NOINIT)) {
#ifdef DEBUG
      Serial.printf("Decoding failed: %s\n", PB_GET_ERROR(&istream_));
#endif
    }
    // directly work on instruction instead of parsing it

    // alternatively expose instruciton in loop to adhere the the setup/loop
    // structure
    // this->executeInstruction(instruction_);
    instruction_queue_.push(&i);
#ifdef DEBUG
    Serial.printf("Execution done");
#endif
  }
}

void VtprotoHandler::onRead(BLECharacteristic *pCharacteristic) {}

void VtprotoHandler::executeInstruction(Instruction &i) {
#ifdef DEBUG
  Serial.print("Instruction type ");
  Serial.println(i.which_concrete_instruction);
#endif
  tact::vtproto::Group *g =
      (tact::vtproto::Group *)i.cb_concrete_instruction.arg;
  switch (i.which_concrete_instruction) {
    case Instruction_add_channels_to_group_tag:
      g->id_ = i.concrete_instruction.add_channels_to_group.group_id;
      /*
          Für die gegebene Gruppen ID den Index des Arrays heraussuchen
          Wenn das Array noch leer ist, dann gibt es keine Gruppen, einfach
         hinzufügen Suche durch alle vorhandenen Elemente nach der Gruppen ID
          Wenn die ID gefunden wurde gib den entsprechenden Wert zurück
          Wenn die ID nicht gefunden wurde gib 0/-1 zurück
      */
      group_manager_.addToGroupList(false, *g);
      // uint8_t index =
      // getOrCreateArrayIndexByGroupId(i.concrete_instruction.add_channels_to_group.group_id);

      // groups_.at(index).channels_ = groups_.at(index).channels_ +
      // g->channels_;
      /* Füge die neuen Channels per + operator zu dem Element im Array@Index
       * hinzu */
      break;
    case Instruction_add_groups_to_group_tag:
      g->id_ = i.concrete_instruction.add_groups_to_group.group_id;
      group_manager_.addToGroupList(true, *g);

      break;
    case Instruction_set_parameter_tag:
      this->setInstantParameter(i.concrete_instruction.set_parameter);
      break;
    case Instruction_interpolate_parameter_tag:
      break;
    default:
      break;
  }
}

// TODO: Make distinctions between instructions more clear!
void VtprotoHandler::setInstantParameter(
    InstInstantlySetParameter instruction) {
  if (instruction.which_parameter ==
      InstInstantlySetParameter_frequency_hz_tag) {
#ifdef DEBUG
    Serial.println("Setting frequency value is not supported by this device");
#endif  // DEBUG_TACTILE

    // output_.setFrequency(instruction.target_id.channel_id,
    //                     instruction.parameter.frequency_hz);
  }
  if (instruction.which_target_id == InstInstantlySetParameter_channel_id_tag) {
#ifdef DEBUG
    Serial.println("Setting channel value");
#endif
    received_state_.setPinValue(instruction.target_id.channel_id,
                                instruction.parameter.intensity);

    // TODO: Refactor into own class and neat funtion
    // Compensates the pwm output values based on the number of active outputs
    for (uint8_t i = 0; i < TACTILE_DISPLAY_NUM_OUTPUTS; i++) {
      compensated_state_.setPinValue(
          i, received_state_.getPinValue(i) == 0
                 ? 0
                 : tact::vtproto::getCompensatedFloat(
                       received_state_.getPinValue(i), COMPENSATION_NEW_MIN,
                       COMPENSATION_NEW_MAX, COMPENSATION_OFFSET,
                       received_state_.getNumberOfActivePins()));
      output_.setIntensity(i, compensated_state_.getPinValue(i));
    }
  } else  // Groups
  {
#ifdef DEBUG
    Serial.println("Setting group value");
#endif
    if (instruction.target_id.group_id == 0) {
      for (uint8_t i = 0; i < output_.getActuatorCount(); i++) {
        output_.setIntensity(i, instruction.parameter.intensity);
      }
    } else {
      // Get Group from groups_ with received group id
      // Loop through all channels in group
      // Loop through all groups in group
      tact::vtproto::Group *group =
          group_manager_.getGroupByGroupId(instruction.target_id.group_id);
      if (group == NULL) {
#ifdef DEBUG
        Serial.println("Group with the specified id not found");
#endif
        return;
      }
      for (uint16_t i = 1; i <= group->channels_.count(); i++) {
        output_.setIntensity(group->channels_.getNth(i),
                             instruction.parameter.intensity);
      }

      // TODO Define recursive way of calling groups from group xP

#ifdef DEBUG
      if (group->groups_.count() > 0)
        Serial.println("Calling groups from groups not supported yet");
#endif
    }
  }
#ifdef DEBUG
  Serial.println("Done setInstantParameter ");
#endif
}
}  // namespace ble
}  // namespace tact