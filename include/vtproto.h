#ifndef VT_PROTO_H_
#define VT_PROTO_H_
#include <Arduino.h>
#include <Array.h>
#include <Set.h>
#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "tacton/groups.h"
#include "vtproto.pb.h"
namespace tact {

namespace vtproto {
/**
 * Returns structs with the neccessary callbacks for encoding the repeated
 * fields already assigned
 */
namespace encode {
bool callbackEncodeString(pb_ostream_t* stream, const pb_field_iter_t* field,
                          void* const* arg);
bool callbackIdsToAddToGroup(pb_ostream_t* stream, const pb_field_iter_t* field,
                             void* const* arg);
FileHeader getHeader(uint32_t format_version, char* pattern_name, char* author,
                     uint32_t duration_ms, uint32_t n_instructions,
                     uint32_t n_channels);
InstInstantlySetParameter _getInstInstantlySetParameterHeader(
    uint8_t which_target_id, uint8_t target_id, uint8_t which_parameter);
InstInstantlySetParameter getInstInstantlySetFrequency(uint8_t which_target_id,
                                                       uint8_t target_id,
                                                       uint32_t frequency_hz);
InstInstantlySetParameter getInstInstantlySetIntensity(uint8_t which_target_id,
                                                       uint8_t target_id,
                                                       float intensity);
InstInterpolateParameter _getInstInterpolateHeader(uint8_t which_target_id,
                                                   uint8_t target_id,
                                                   uint8_t which_parameter);
InstInterpolateParameter getInstInterpolateIntenisty(uint8_t which_target_id,
                                                     uint8_t target_id,
                                                     float intensity,
                                                     uint32_t ms);
InstInterpolateParameter getInstInterpolateFrequency(uint8_t which_target_id,
                                                     uint8_t target_id,
                                                     uint32_t frequency_hz,
                                                     uint32_t ms);
InstAddToGroup getInstAddToGroup(uint32_t group_id, Set& ids);
InstWait getInstWait(uint32_t ms);
// InstAddToGroup getInstAddToGroup(Group &group);
}  // namespace encode

/**
 * Returns structs with the neccessary callbacks for decoding and populating the
 * repeated fields already assigned
 */
namespace decode {
bool callbackDecodeString(pb_istream_t* stream, const pb_field_t* field,
                          void** arg);
bool callbackAddIdToGroup(pb_istream_t* stream, const pb_field_iter_t* field,
                          void** arg);
bool callbackInstruction(pb_istream_t* stream, const pb_field_iter_t* field,
                         void** arg);
FileHeader getHeader(char* pattern_name_buf, char* author_buf);
// InstAddToGroup getInstAddToGroup(Group &group);
Instruction getInstruction(Group& group);
InstAddToGroup getInstAddChannelsToGroup(Group& group);
InstAddToGroup getInstAddGroupsToGroup(Group& group);
// FileHeader getHeader(char *pattern_name, char *author);

}  // namespace decode
}  // namespace vtproto
}  // namespace tact
#endif  // !VT_PROTO_H_