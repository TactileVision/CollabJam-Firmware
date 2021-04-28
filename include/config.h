#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>
namespace tact
{
    namespace display
    {
        const uint8_t number_of_motors = 32;

    }
    namespace vtproto {
        const uint8_t number_of_outputs = tact::display::number_of_motors;
    }
}
#endif // !CONFIG_H_