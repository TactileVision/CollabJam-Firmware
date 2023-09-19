#include "ble/ble_services.h"

namespace tact {
namespace ble {
namespace vtproto_service {
BleCharacteristicDescription characteristic_descriptions[] = {
    {"Num_Actuators",
     "f33c0002-8ebf-4c9c-83ec-bfff479a930b",
     kBLE_PERMISSION_READ,
     {0x00, 0x00},
     kBLE_CPF_UINT32},
    {"Actuators_can_Amplitude",
     "f33c0003-8ebf-4c9c-83ec-bfff479a930b",
     kBLE_PERMISSION_READ,
     {0x00, 0x00},
     kBLE_CPF_UINT32},
    {"Actuators_can_Frequency",
     "f33c0004-8ebf-4c9c-83ec-bfff479a930b",
     kBLE_PERMISSION_READ,
     {0x00, 0x00},
     kBLE_CPF_UINT32},
    {"Amplitude",
     "f33c0032-8ebf-4c9c-83ec-bfff479a930b",
     kBLE_PERMISSION_WRITE | kBLE_PERMISSION_WRITEWITHOUTRESPONSE,
     {0x01, 0x01},
     kBLE_CPF_STRUCT},
    {"Frequency",
     "f33c0033-8ebf-4c9c-83ec-bfff479a930b",
     kBLE_PERMISSION_WRITE | kBLE_PERMISSION_WRITEWITHOUTRESPONSE,
     {0x01, 0x01},
     kBLE_CPF_STRUCT},
};
}  // namespace vtproto_service

namespace service_hm10 {

BleCharacteristicDescription
    characteristic_descriptions[kNumberOfCharacteristics] = {
        {"RX_Buffer",
         "0000ffe1-0000-1000-8000-00805f9b34fb",
         (kBLE_PERMISSION_READ | kBLE_PERMISSION_WRITE |
          kBLE_PERMISSION_NOTIFY),
         {0x00, 0x01},
         kBLE_CPF_STRUCT},  // Receive Buffer
        {"TX_Buffer",
         "00ee4dd0-b68d-459f-b67d-bd6ea3297dd0",
         (kBLE_PERMISSION_READ | kBLE_PERMISSION_NOTIFY),
         {0x00, 0x01},
         kBLE_CPF_STRUCT}  // Send Buffer
};
}
}  // namespace ble

}  // namespace tact