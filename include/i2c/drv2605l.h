#ifndef TACT_DRV2605L_H_
#define TACT_DRV2605L_H_
#include <Arduino.h>

#include "Adafruit_DRV2605.h"


constexpr unsigned int kSamplingTimes[4] = {150, 200, 250, 300};
constexpr unsigned int kBlankingAndIdissTimes[16] = {
    15, 25, 50, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 235, 260, 285};
typedef enum MotorType { kERM, kLRA } MotorType;

enum class OutputSource { kPwm = 0x03, kI2cRealtime = 0x05, kLib };
enum class LoopMode { kClosedLoop = 0x00, kOpenLoop };
typedef struct ActuatorConfig {
  MotorType motor_type;
  float rated_voltage;
  float clamp_voltage;
  uint16_t frequency;

  bool operator==(const ActuatorConfig& c) {
    return (rated_voltage == c.rated_voltage &&
            clamp_voltage == c.clamp_voltage && frequency == c.frequency);
  }

} ActuatorConfig;

// struct ActuatorConfigStateChangeFlags {
//   bool motor_type = false;
//   bool rated_voltage = false;
//   bool clamp_voltage = false;
//   bool frequency = false;
// };

// void checkForStateChanges(ActuatorConfigStateChangeFlags did_change,
//                           ActuatorConfig& first, ActuatorConfig& second);

typedef struct DrvConfig {
  // vorzeichen
  uint8_t mode;
  uint8_t pattern_library;
  LoopMode loop_mode;
  bool rtp_mode_unsigned;

  bool operator==(const DrvConfig& c) {
    return (mode == c.mode && pattern_library == c.pattern_library &&
            loop_mode == c.loop_mode &&
            rtp_mode_unsigned == c.rtp_mode_unsigned);
  }

  // bool operator!=(const DrvConfig& c) { return !(this == c); }
} DrvConfig;

class DRV2605L {
 private:
  /* data */
  bool drvWriteAndCheck(uint8_t reg, uint8_t value);

 public:
  DRV2605L();
  ~DRV2605L();

  Adafruit_DRV2605 drv_;

  /**
   * Initite the Adafruit DRV library, which in turn initialises the I2C setup.
   * Must be called before working with the class
   */
  void begin();

  /**
   *
   * Configures the DRV250L settings to drv_config
   * parameter.
   * @param drv_config Output mode and pattern library to be used
   */
  void setupDRV(DrvConfig drv_config);

  /**
   * Configures the actuator settings in the DRV according to motor_config
   * parameter. Depending on the Loop mode,
   * clamp and rated voltage are interpreted differently, check the datasheet
   * p.24ff for further information.
   * @param motor_config The actuator type (ERM or LRA), the rated voltage, the
   * clamp voltage and the frequency (only lra)
   */
  void setupActuator(ActuatorConfig motor_config);

  /**
   * Sets the voltage according to the formula for closed loop voltage
   * (datasheet p.24ff).
   */
  void setVoltagesClosedLoop(ActuatorConfig motor_config);

  /**
   * Sets the voltage according to the formula for open loop voltage (datasheet
   * p.24). Clamp voltage will be used as reference voltage in this instance.
   */
  void setVoltagesOpenLoop(ActuatorConfig motor_config);

  /**
   * Sets the drive frequency of the lra actuator, according to the formula from
   * the datasheet p.53
   */
  void setLraFreqOpenLoop(uint16_t freq);

  /**
   * Specify and trigger one of the available library pattern (datasheet p.63).
   * Device must be in INTTRIG mode.
   */
  void triggerLibOutput(unsigned int pattern_number);

  /** Sets the device to use Realtime-Playback-Mode (datasheet p.17), either
   * signed or unsigned (p.30ff,51)*/
  void useRtpMode(bool unsigned_mode);
  // void output(float value);
  // void output(uint8_t value);

  void setLoopMode(LoopMode mode);
  /**
   * Sets both ERM and LRA to open loop mode, check datasheet for implications
   */
  void useOpenLoopMode();

  /**
   * Sets both ERM and LRA to closed loop mode, check datasheet for implications
   */
  void useClosedLoopMode();
};


#endif  // !TACT_DRV2605L_H_