#include "hardware_interfaces/cj2_hw_interface.h"

CollabJamV2HardwareInterface::CollabJamV2HardwareInterface()
    : actor_{0x48, 0x49, 0x4a, 0x4b} {};
CollabJamV2HardwareInterface::~CollabJamV2HardwareInterface(){};

void CollabJamV2HardwareInterface::init() {
  // TODO Setup DA7280 here
  hapticSettings settings = {LRA_TYPE, 2.5, 2, 350, 6.5, 170};

  for (uint8_t i = 0; i < 4; i++) {
    if (!actor_[i].begin())
      DEBUG_PRINTLN("[CJ2] Could not communicate with Haptic Driver.");
    else
      DEBUG_PRINTLN("[CJ2] Qwiic Haptic Driver DA7280 found!");

    // if (!actor_[i].defaultMotor())
    //   DEBUG_PRINTLN("[CJ2] Could not set default settings.");

    actor_[i].setMotor(settings);
    actor_[i].enableAmpPid(false);
    actor_[i].enableFreqTrack(false);
    actor_[i].enableAcceleration(false);
    actor_[i].enableRapidStop(false);
    actor_[i].setOperationMode(DRO_MODE);
  }
}

void CollabJamV2HardwareInterface::setIntensity(uint8_t channel, float value) {
  if (channel > 4) return;
}
void CollabJamV2HardwareInterface::setIntensity(uint8_t channel,
                                                uint8_t value) {
  if (channel > 4) return;
  int x = map(value, 0, 254, 0, 127);
  actor_[channel].setVibrate(x);
}
void CollabJamV2HardwareInterface::setFrequency(uint8_t channel,
                                                uint32_t value) {
  if (channel > 4) return;
  actor_[channel].setActuatorLRAfreq(value);
}