#include "ble/ble_util.h"

namespace tact {

namespace ble {

void createCharacteristicsFromDescription(
    BLEService *service, BLECharacteristic **characteristic,
    BleCharacteristicDescription *description, uint8_t length) {
  BLE2904 *ble2904;
  BLEDescriptor *ble_desc;
  for (uint8_t i = 0; i < length; i++) {
    characteristic[i] = service->createCharacteristic(
        description[i].uuid_, description[i].access_property);

    // CCCD values
    ble_desc = new BLEDescriptor(BLEUUID((uint16_t)kBLE_UUID_CCCD));
    ble_desc->setValue(description[i].cccd_values, 2);
    characteristic[i]->addDescriptor(ble_desc);

    // // User description
    ble_desc = new
    BLEDescriptor(BLEUUID((uint16_t)kBLE_UUID_USER_DESCRIPTION));
    ble_desc->setValue(description[i].user_description);
    characteristic[i]->addDescriptor(ble_desc);

    // Presentation format
    // TODO Make complete cpf
    // ble2904 = new BLE2904();
    // ble2904->setUnit(description[i].presentation_format);
    // characteristic[i]->addDescriptor(ble2904);
  }
}

void BleConnectionCallback::onConnect(BLEServer *pServer) {
#ifdef DEBUG
  Serial.println("Device connected to server");
  BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_CONN_HDL0);
#endif  // DEBUG_TACTILE
}
void BleConnectionCallback::onDisconnect(BLEServer *pServer) {
#ifdef DEBUG
  Serial.println("Device disconnected from server\n");
#endif  // DEBUG_TACTILE
  BLEDevice::startAdvertising();
}

}  // namespace ble
}  // namespace tact
