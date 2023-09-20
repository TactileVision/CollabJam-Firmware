#ifndef PCA9547_H_
#define PCA9547_H_
#include "Wire.h"
namespace tact {

class PCA9547 {
 private:
  uint8_t address_, error_, data_, state_ = 0x00;
  void write(uint8_t reg);

 public:
  void setup(uint8_t address);
  void select(uint8_t channel);
  void deselectAll();
  uint8_t read();
  PCA9547();
  ~PCA9547();
};

}  // namespace tact
#endif  // !TCA