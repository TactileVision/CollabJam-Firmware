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

void addToGroup()
{

  //Create dummy data
  uint32_t enc_ids[tact::display::number_of_motors], dec_ids[tact::display::number_of_motors];
  for (uint8_t i = 0; i < tact::display::number_of_motors; i++)
  {
    enc_ids[i] = i;
  }
  tact::vtproto::output_id_list_t a, b;
  a.ids_ = enc_ids;
  a.length_ = 32;

  b.ids_ = dec_ids;
  b.length_ = 0;

  InstAddToGroup enc_inst_add_to_group = tact::vtproto::makeEncodingInstAddToGroup(1, &a);
  InstAddToGroup dec_inst_add_to_group = tact::vtproto::makeDecodingInstAddToGroup(&b);

  pb_ostream_t pb_ostream;
  pb_ostream = pb_ostream_from_buffer(tact::vtproto::vt_message_buffer, sizeof(tact::vtproto::vt_message_buffer));

  // pb_encode(&pb_ostream, InstAddToGroup_fields, &enc_inst_add_to_group);
  pb_encode_ex(&pb_ostream, InstAddToGroup_fields, &enc_inst_add_to_group, PB_ENCODE_DELIMITED);

  pb_istream_t pb_istream;
  pb_istream = pb_istream_from_buffer(tact::vtproto::vt_message_buffer, pb_ostream.bytes_written);

  Serial.printf("Bytes written: %i\n", pb_ostream.bytes_written);
  Serial.printf("Bytes left: %i\n", pb_istream.bytes_left);

  // if (!pb_decode(&pb_istream, InstAddToGroup_fields, &dec_inst_add_to_group))
  if (!pb_decode_ex(&pb_istream, InstAddToGroup_fields, &dec_inst_add_to_group, PB_DECODE_DELIMITED))
  {
    const char *error = PB_GET_ERROR(&pb_istream);
    Serial.printf("pb_decode error: %s", error);
    return;
  }
}

/* The callback below is a message-level callback which is called before each
 * submessage is encoded. It is used to set the pb_callback_t callbacks inside
 * the submessage. The reason we need this is that different submessages share
 * storage inside oneof union, and before we know the message type we can't set
 * the callbacks without overwriting each other.
 */
// bool decode_and_store(pb_istream_t *stream, const pb_field_t *field, void **arg)
// bool decode_and_directly_output(pb_istream_t *stream, const pb_field_t *field, void **arg)
bool decode_instruction_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  //https://github.com/nanopb/nanopb/blob/master/tests/oneof_callback/decode_oneof.c
  if (field->tag == Instruction_add_groups_to_group_tag)
  {
    InstAddToGroup *f = field->pData;
    // f->ids_to_be_added.funcs.decode =
  }
  else if (field->tag == Instruction_add_channels_to_group_tag)
  {
    InstAddToGroup *f = field->pData;
    // f->ids_to_be_added.funcs.decode =
  }
  else if (field->tag == Instruction_wait_tag)
  {
  }
  else if (field->tag == Instruction_set_waveform_type_tag)
  {
  }
  else if (field->tag == Instruction_set_parameter_tag)
  {
    // InstInstantlySetParameter *f = field->pData;
    //Do in need to do something here?
  }
  else if (field->tag == Instruction_interpolate_parameter_tag)
  {
  }

  /* Once we return true, pb_dec_submessage() will go on to decode the
     * submessage contents. But if we want, we can also decode it ourselves
     * above and leave stream->bytes_left at 0 value, inhibiting automatic
     * decoding.
     */
  return true;
}

void instantlySetParameter()
{
  //Create dummy data
  InstInstantlySetParameter enc = InstInstantlySetParameter_init_default;
  enc.which_parameter = InstInstantlySetParameter_intensity_tag;
  enc.parameter.intensity = 0.33f;
  enc.which_target_id = InstInstantlySetParameter_channel_id_tag;
  enc.target_id.channel_id = 1;

  InstInstantlySetParameter dec = InstInstantlySetParameter_init_default;

  //Encode data and write into buffer
  pb_ostream_t pb_ostream;
  pb_ostream = pb_ostream_from_buffer(tact::vtproto::vt_message_buffer, sizeof(tact::vtproto::vt_message_buffer));
  pb_encode_ex(&pb_ostream, InstInstantlySetParameter_fields, &enc, PB_ENCODE_DELIMITED);
  // pb_encode(&pb_ostream, InstInstantlySetParameter_fields, &enc);

  //Read data from buffer and decode
  pb_istream_t pb_istream;
  pb_istream = pb_istream_from_buffer(tact::vtproto::vt_message_buffer, pb_ostream.bytes_written);
  Serial.printf("Bytes written: %i\n", pb_ostream.bytes_written);
  Serial.printf("Bytes left: %i\n", pb_istream.bytes_left);
  // if (!pb_decode(&pb_istream, InstInstantlySetParameter_fields, &dec))
  if (!pb_decode_ex(&pb_istream, InstInstantlySetParameter_fields, &dec, PB_DECODE_DELIMITED))
  {
    const char *error = PB_GET_ERROR(&pb_istream);
    Serial.printf("pb_decode error: %s", error);
    return;
  }
  Serial.printf("Decoded: %f @ %i\n", dec.parameter.intensity, dec.target_id.channel_id);
}

void instructionAddToGroup()
{
  //Create dummy data
  uint32_t enc_ids[tact::display::number_of_motors], dec_ids[tact::display::number_of_motors];
  for (uint8_t i = 0; i < tact::display::number_of_motors; i++)
  {
    enc_ids[i] = i;
  }
  tact::vtproto::output_id_list_t a, b;
  a.ids_ = enc_ids;
  a.length_ = 32;
  b.ids_ = dec_ids;
  b.length_ = 0;

  InstAddToGroup enc_inst_add_to_group = tact::vtproto::makeEncodingInstAddToGroup(1, &a);
  InstAddToGroup dec_inst_add_to_group = tact::vtproto::makeDecodingInstAddToGroup(&b);
  Instruction enc_instruction = Instruction_init_default;
  enc_instruction.which_concrete_instruction = Instruction_add_channels_to_group_tag;
  enc_instruction.concrete_instruction.add_channels_to_group = enc_inst_add_to_group;

  Instruction dec_instruction = Instruction_init_default;

  pb_ostream_t pb_ostream;
  pb_ostream = pb_ostream_from_buffer(tact::vtproto::vt_message_buffer, sizeof(tact::vtproto::vt_message_buffer));

  pb_encode_ex(&pb_ostream, Instruction_fields, &enc_instruction, PB_ENCODE_DELIMITED);
  // pb_encode(&pb_ostream, Instruction_fields, &enc_instruction);

  pb_istream_t pb_istream;
  pb_istream = pb_istream_from_buffer(tact::vtproto::vt_message_buffer, pb_ostream.bytes_written);

  Serial.printf("Bytes written: %i\n", pb_ostream.bytes_written);
  Serial.printf("Bytes left: %i\n", pb_istream.bytes_left);
  // if (!pb_decode(&pb_istream, Instruction_fields, &dec_instruction))
  if (!pb_decode_ex(&pb_istream, Instruction_fields, &dec_instruction, PB_DECODE_DELIMITED))
  {
    const char *error = PB_GET_ERROR(&pb_istream);
    Serial.printf("pb_decode error: %s", error);
    return;
  }
  //How to find out which type of instruction is in decoded instruction
  Serial.printf("decoded construction with tag number %i\n", dec_instruction.which_concrete_instruction);
  tact::vtproto::printInstructionType(dec_instruction.which_concrete_instruction);
  Serial.printf("info %i\n", b.length_);
  for (uint8_t i = 0; i < b.length_; i++)
  {
    Serial.println(b.ids_[i]);
  }
}

void instructionInstantlySetParameter()
{

  InstInstantlySetParameter enc_set_parameter = InstInstantlySetParameter_init_default;
  enc_set_parameter.which_parameter = InstInstantlySetParameter_intensity_tag;
  enc_set_parameter.parameter.intensity = 0.33f;
  enc_set_parameter.which_target_id = InstInstantlySetParameter_channel_id_tag;
  enc_set_parameter.target_id.channel_id = 1;

  Instruction enc_instruction = Instruction_init_default;

  InstInstantlySetParameter dec_set_parameter = InstInstantlySetParameter_init_default;
  Instruction dec_instruction = Instruction_init_default;

  enc_instruction.which_concrete_instruction = Instruction_set_parameter_tag;
  enc_instruction.concrete_instruction.set_parameter = enc_set_parameter;

  pb_ostream_t pb_ostream;
  pb_ostream = pb_ostream_from_buffer(tact::vtproto::vt_message_buffer, sizeof(tact::vtproto::vt_message_buffer));

  pb_encode_ex(&pb_ostream, Instruction_fields, &enc_instruction, PB_ENCODE_DELIMITED);
  // pb_encode(&pb_ostream, Instruction_fields, &enc_instruction);

  pb_istream_t pb_istream;
  pb_istream = pb_istream_from_buffer(tact::vtproto::vt_message_buffer, pb_ostream.bytes_written);

  Serial.printf("Bytes written: %i\n", pb_ostream.bytes_written);
  Serial.printf("Bytes left: %i\n", pb_istream.bytes_left);
  // if (!pb_decode(&pb_istream, Instruction_fields, &dec_instruction))
  if (!pb_decode_ex(&pb_istream, Instruction_fields, &dec_instruction, PB_DECODE_DELIMITED))
  {
    const char *error = PB_GET_ERROR(&pb_istream);
    Serial.printf("pb_decode error: %s", error);
    return;
  }
  //How to find out which type of instruction is in decoded instruction
  Serial.printf("decoded construction with tag number %i\n", dec_instruction.which_concrete_instruction);
  tact::vtproto::printInstructionType(dec_instruction.which_concrete_instruction);
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

  // addToGroup();
  // instantlySetParameter();
  // instructionInstantlySetParameter();
  instructionAddToGroup();
}

void loop()
{
  // put your main code here, to run repeatedly:
}
