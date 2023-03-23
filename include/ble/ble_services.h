#ifndef BLE_SERVICES_H_
#define BLE_SERVICES_H_
#include "ble/ble_util.h"
namespace tact {
namespace ble {
namespace vtproto_service {
const uint8_t kChrNumberOfOutputs = 0x00;
const uint8_t kChrDisplayPlaybackState = 0x01;
const uint8_t kChrDisplayPlaybackRequestStop = 0x02;
const uint8_t kChrModeVtprotoMaxMsgLength = 0x03;
const uint8_t kChrModeVtprotoBuffer = 0x04;
const uint8_t kChrDisplayConfig = 0x05;
const uint8_t kChrConfigAvailableOptions = 0x06;
const uint8_t kChrConfigChangeBuffer = 0x07;
const uint8_t kChrConfigChangeBufferMaxLength = 0x08;
const uint8_t kChrStoredTactonsList = 0x09;
const uint8_t kChrStoredTactonPlaybackRequestMaxLength = 0x0A;
const uint8_t kChrStoredTactonPlaybackRequest = 0x0B;

// Generic
const uint8_t kNumberOfCharacteristics = 5;
const std::string kServiceUuid = "f33c0001-8ebf-4c9c-83ec-bfff479a930b";
extern BleCharacteristicDescription
    characteristic_descriptions[kNumberOfCharacteristics];
};  // namespace vtproto_service

namespace service_hm10 {

const uint8_t kChrRXBuffer = 0x00;
const uint8_t kChrTXBuffer = 0x01;
const std::string kServiceUuid = "5eb8eec2-b92d-4dca-901c-3bc3b69936e6";
static const uint8_t kNumberOfCharacteristics = 2;
extern BleCharacteristicDescription
    characteristic_descriptions[kNumberOfCharacteristics];
}  // namespace service_hm10

}  // namespace ble
}  // namespace tact
#endif  // !BLE_SERVICES_H_