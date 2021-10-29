#include "tacton_player_esp.h"

namespace tact {

namespace vtproto {

TactonPlayerESP::TactonPlayerESP(HardwareInterface& output) : output_(output){};

void TactonPlayerESP::play(Tacton* t) {
  /*   #ifdef DEBUG
    t->print();
    #endif  // DEBUG */
  this->timing_.t_elapsed_ = 0;
  this->timing_.t_0_ = millis();

  for (uint16_t i = 0; i < t->header_.file_header_.n_instructions; i++) {
    switch (t->instructions_[i].which_concrete_instruction) {
      case Instruction_add_channels_to_group_tag:
#ifdef DEBUG
        Serial.println("add to channel");
#endif  // DEBUG
        break;
      case Instruction_add_groups_to_group_tag:
#ifdef DEBUG
        Serial.println("add to group");
#endif  // DEBUG
        break;
      case Instruction_wait_tag:
#ifdef DEBUG
        Serial.println("wait");
#endif  // DEBUG
        while (this->timing_.t_elapsed_ <
               t->instructions_[i].concrete_instruction.wait.milliseconds) {
          this->timing_.t_elapsed_ = millis() - this->timing_.t_0_;
        }
#ifdef DEBUG
        Serial.print("waited ");
        Serial.println(this->timing_.t_elapsed_);
#endif  // DEBUG
        this->timing_.t_elapsed_ = 0;
        this->timing_.t_0_ = millis();
        break;
      case Instruction_set_waveform_type_tag:
#ifdef DEBUG
        Serial.println("set waveform");
#endif  // DEBUG
        break;
      case Instruction_set_parameter_tag:
        if (t->instructions_[i]
                .concrete_instruction.set_parameter.which_parameter ==
            InstInstantlySetParameter_intensity_tag) {
          if (t->instructions_[i]
                  .concrete_instruction.set_parameter.which_target_id ==
              InstInstantlySetParameter_channel_id_tag) {
#ifdef DEBUG
            Serial.print("setting channel ");
            Serial.print(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.target_id.channel_id);
            Serial.print(" intensity to ");
            Serial.println(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.parameter.intensity);
#endif  // DEBUG
            this->output_.setIntensity(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.target_id.channel_id,
                t->instructions_[i]
                    .concrete_instruction.set_parameter.parameter.intensity);
          } else {
            Group* g = t->group_manager_.getGroupByGroupId(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.target_id.group_id);
#ifdef DEBUG
            Serial.print("setting groups ");
            Serial.print(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.target_id.group_id);
            Serial.print(" intensity to ");
            Serial.print(
                t->instructions_[i]
                    .concrete_instruction.set_parameter.parameter.intensity);
            Serial.print("(");
            Serial.print(g->channels_.count());
            Serial.println(" Channels)");
#endif  // DEBUG
            ;
            for (uint8_t j = 0; j < g->channels_.count(); j++) {
              this->output_.setIntensity(
                  g->channels_.getNth(j + 1),
                  t->instructions_[i]
                      .concrete_instruction.set_parameter.parameter.intensity);
            }

#ifdef DEBUG
            Serial.println("set intensity of group");
#endif  // DEBUG
          }
        } else {
          if (t->instructions_[i]
                  .concrete_instruction.set_parameter.which_target_id ==
              InstInstantlySetParameter_channel_id_tag) {
#ifdef DEBUG
            Serial.println("set frequency of channel");
#endif  // DEBUG
          } else {
#ifdef DEBUG
            Serial.println("set frequency of group");
#endif  // DEBUG
          }
        }
        break;
      case Instruction_interpolate_parameter_tag:
#ifdef DEBUG
        Serial.println("interpolate parameter");
#endif  // DEBUG
        break;
      default:
        break;
    }
    /* code */
    // Set instructions until wait instruction comes
    // Track time intervals between each wait instruction
    // subtract alredy passed time from wait duration and keep waiting until it
    // is reached do next step
  }
}
}  // namespace vtproto
}  // namespace tact
