#include "hardware_interfaces/cj4_hw_interface.h"

CollabJamV4HardwareInterface::CollabJamV4HardwareInterface()
    : actor_{0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a} {};
CollabJamV4HardwareInterface::~CollabJamV4HardwareInterface(){};

void CollabJamV4HardwareInterface::init() {
  mp_.setup(0x70);
  hapticSettings settings = {LRA_TYPE, 2.5, 2, 350, 6.5, 170};
  // hapticSettings settings = {LRA_TYPE, 2.5, 2, 350, 6.5, 170};
  // hapticSettings settings = {LRA_TYPE, 1.5, 1, 200, 5, 50};
  // hapticSettings settings = {ERM_TYPE, 3.3, 3, 200, 30, 50};

  for (uint8_t i = 0; i < 6; i++) {
    mp_.select(i);
    if (!actor_[i].begin())
      DEBUG_PRINTLN("[cj4] Could not communicate with Haptic Driver.");
    else
      DEBUG_PRINTLN("[cj4] Qwiic Haptic Driver DA7280 found!");

    // if (!actor_[i].defaultMotor())
    //   DEBUG_PRINTLN("[cj4] Could not set default settings.");

    actor_[i].setMotor(settings);
    actor_[i].enableAmpPid(false);
    actor_[i].enableFreqTrack(false);
    actor_[i].enableAcceleration(false);
    actor_[i].enableRapidStop(false);
    actor_[i].setOperationMode(DRO_MODE);
  }
}

void CollabJamV4HardwareInterface::setIntensity(uint8_t channel, float value) {
  if (channel > 6) return;
}
void CollabJamV4HardwareInterface::setIntensity(uint8_t channel,
                                                uint8_t value) {
  if (channel > 6) return;

  int x = map(value, 0, 254, 0, 127);
  mp_.select(channel);
  actor_[channel].setVibrate(x);
}
void CollabJamV4HardwareInterface::setFrequency(uint8_t channel,
                                                uint32_t value) {
  if (channel > 6) return;
  mp_.select(channel);
  actor_[channel].setActuatorLRAfreq(value);
}