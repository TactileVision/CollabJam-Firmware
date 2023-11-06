#ifndef _TV_MULTIPLEXED_DRV_H_
#define _TV_MULTIPLEXED_DRV_H_

// TODO Create ENUM for selection of erm and lra

#include "debug_util.h"
#include "i2c/drv2605l.h"

#ifdef TD_PCA
#include "i2c/pca9547.h"
#else
#include "i2c/tca9548a.h"
#endif

/**
 *
 * Combines a i2c multiplexer and a number of DRV2605L haptic motor driver, that
 * are connected to that multiplexer, into a single class. Provides functions
 * that work on all DRV's at once. If single DRV's have to be configured, the
 * public member variables can be used to access them indivdually by selecting
 * the desired channel with the multiplexer and executing the changes through
 * the drv_ member.
 */
class MultiplexedDrv {
 public:
  static const unsigned int kNumberOfActuators;
  DRV2605L drv_;
  uint8_t mp_addr_ = 0x70;

// Specify the multiplexer used by setting the debug flag for the used
// enviornment in platformio.ini. Datasheets for both multiplexers are located
// in the doc folder
#ifdef TD_PCA
  tact::PCA9547 multiplexer_;
#else
  tact::TCA9548A multiplexer_;
#endif

  /**
   * Initializes the multiplexer and all DRV channels. Configures the DRV
   * channels to use the specified motor type and the appropriate tacton library
   * based on the motor type
   *MUST be called before interfacing with said devices; call in `setup()`
   function.

   * @param use_lra Specifies for all channels if LRA motors(true) or ERM
   * motors(false) are used.
   */
  void setMultiplexerAddress(uint8_t adddr);
  void initAllDrvs(bool use_lra);  // TODO (individual) useLRA/useERM
  void initAllDrvs(bool* use_lra_per_channel, uint8_t len);
  void initAllDrvs(ActuatorConfig actutator_config,
                   DrvConfig drv_config);

  void updateAllSettings(ActuatorConfig actutator_config,
                         DrvConfig drv_config);

  /**Plays the 'Strong Click - 100%' pattern from the the DRV2605L pattern
   * library (datasheet p.63ff) on each DRV.
   */
  void pingAllOutputs();

  /**
    Sets all DRV2605L to use realtime mode (datasheet p. 17), where the
    vibration strength is set by an internal register on the DRV2605L. The
    register value can be set by using drv_.setRealtimeValue() function */
  void useAllRtpMode(bool unsigned_mode);

  /**   Sets all DRV2605L to use waveform sequencer mode (datasheet p. 16),
    where the vibration is set by up to eight registers on the DRV2605L, where
    each register can be used to either specift a pattern number or a waiting
    time in milliseconds * 10 (datasheet p.33ff.). The register valuse can be
    set by using drv_.drv_.setWaveform() function */
  void useAllLibMode();

  /**
   * Sets the GO Bit in Register 0x0C for all connected DRVs to 0. Setting the
   * go bit to zero stops all outputs that are triggered by it (waveform
   * sequence lib)
   */
  void setAllGoBitsLow();
  // void setupLRA(float rated_voltage, float clamp_voltage, uint16_t
  // frequency);

  /**
   * Sets both realtime playback and library pattern output sources to 0, so
   *that no vibration will occur
   **/
  void stopAllOutputs();

  /**
   * Sets allrealtime playback output sources to 0, so that no vibration will
   *occur
   **/
  void stopRtpOutputs();

  /**
   * Sets all library pattern output sources to 0, so that no vibration will
   *occur
   **/
  void stopLibOutputs();

  MultiplexedDrv();
  ~MultiplexedDrv();

 private:
  /**
 Triggers one of the available pattern from the DRV2605L pattern library
 (datasheet p.63ff). Before callling, select the desired DRV chip by using
 `multiplexer.select(num)`. Up to eight waveforms can be sequenced in the DRV
 chip (datasheet p.34, p.39)
 @param pattern_number The DRV2605L pattern lib number (1-123)
 @see datasheet p.63ff
 */
  void triggerLibOutput(unsigned int pattern_number);
};
// TODO Rename stop() in drv player classes to reset()/stopAndReset()
#endif