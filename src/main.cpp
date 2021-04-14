#include <Arduino.h>
#include <ble_esp.h>
#include <ble_services.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <vtproto.pb.h>
#include <vtproto.h>
#include "config.h"

namespace tact
{
  namespace vtproto
  {
    const uint16_t kBufferSize = 2048;
    uint8_t vt_message_buffer[kBufferSize];

  }
}
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    /* code */
  }
  using namespace tact::ble;
  BLEDevice::init("device name");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new tact::ble::BleConnectionCallback());

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  // Setup Service and its Characteristics
  BLEService *service = server->createService(service_tactile_display::kServiceUuid, 1 + (service_tactile_display::kNumberOfCharacteristics * 5));
  BLECharacteristic *characteristics[service_tactile_display::kNumberOfCharacteristics];
  createCharacteristicsFromDescription(service, characteristics, service_tactile_display::characteristic_descriptions, service_tactile_display::kNumberOfCharacteristics);

  // Assign Values to Characteristics
  characteristics[service_tactile_display::kCharacteristicNumberOfOutputs]->setValue((uint8_t *)&tact::display::number_of_motors, 1);
  characteristics[service_tactile_display::kCharacteristicDisplayPlaybackState]->setValue("IDLE");
  // characteristics[service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setValue((uint16_t)0); // No need to write?
  //TODO Check if value of constant is equal to propagated value
  characteristics[service_tactile_display::kCharacteristicModeVtprotoMaxMsgLength]->setValue((uint16_t &)tact::vtproto::kBufferSize);

  // Assign Callbacks to Characteristics
  characteristics[service_tactile_display::kCharacteristicModeVtprotoBuffer]->setCallbacks(new service_tactile_display::VtprotoBufferCallback());
  characteristics[service_tactile_display::kCharacteristicDisplayPlaybackRequestStop]->setCallbacks(new service_tactile_display::VtprotoBufferCallback());

  service->start();
  BLEDevice::startAdvertising();

  //Create dummy data
  uint32_t add_ids_to_group[tact::display::number_of_motors], encoded_ids[tact::display::number_of_motors];
  for (uint8_t i = 0; i < tact::display::number_of_motors; i++)
  {
    add_ids_to_group[i] = i;
  }
  tact::vtproto::output_id_list_t a, b;
  a.ids_ = add_ids_to_group;
  a.length_ = 32;

  b.ids_ = encoded_ids;
  b.length_ = 0;

  InstAddToGroup add_to_group = tact::vtproto::makeEncodingInstAddToGroup(1, &a);
  InstAddToGroup encoded_add_to_group = tact::vtproto::makeDecodingInstAddToGroup(&b);

  // InstInstantlySetParameter set_parameter = InstInstantlySetParameter_init_zero;
  // Instruction i_group = {Instruction_add_channels_to_group_tag, {add_to_group}};
  // Instruction i_group, i_p;
  // i_p.which_concrete_instruction = Instruction_set_parameter_tag;
  // i_p.concrete_instruction.set_parameter = set_parameter;
  // i_group.which_concrete_instruction = Instruction_add_channels_to_group_tag;
  // i_group.concrete_instruction.add_channels_to_group = add_to_group;

  // pb_encode_ex(&pb_output_stream, &InstAddToGroup_msg, &add_to_group, PB_ENCODE_DELIMITED);
  pb_ostream_t pb_output_stream;
  pb_output_stream = pb_ostream_from_buffer(tact::vtproto::vt_message_buffer, sizeof(tact::vtproto::vt_message_buffer));
  pb_encode(&pb_output_stream, InstAddToGroup_fields, &add_to_group);

  pb_istream_t pb_input_stream;
  pb_input_stream = pb_istream_from_buffer(tact::vtproto::vt_message_buffer, pb_output_stream.bytes_written);
  Serial.printf("Bytes written: %i\n", pb_output_stream.bytes_written);
  Serial.printf("Bytes left: %i\n", pb_input_stream.bytes_left);
  if (!pb_decode(&pb_input_stream, InstAddToGroup_fields, &encoded_add_to_group))
  {
    const char *error = PB_GET_ERROR(&pb_input_stream);
    Serial.printf("pb_decode error: %s", error);
    return;
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}
