#ifndef TCA9548A_H_
#define TCA9548A_H_
#include "Wire.h"
namespace tact {

class TCA9548A {
 private:
  uint8_t address_, error_, data_, state_ = 0x00;
  void write(uint8_t reg);

 public:
  void setup(uint8_t address);
  void select(uint8_t channel);
  void deselectAll();
  uint8_t getState();
  uint8_t read();
  TCA9548A();
  ~TCA9548A();
};

}  // namespace tact
#endif  // !TCA