/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: peripherals.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "peripherals.pb-c.h"
void   directional_vector__init
                     (DirectionalVector         *message)
{
  static const DirectionalVector init_value = DIRECTIONAL_VECTOR__INIT;
  *message = init_value;
}
size_t directional_vector__get_packed_size
                     (const DirectionalVector *message)
{
  assert(message->base.descriptor == &directional_vector__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t directional_vector__pack
                     (const DirectionalVector *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &directional_vector__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t directional_vector__pack_to_buffer
                     (const DirectionalVector *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &directional_vector__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
DirectionalVector *
       directional_vector__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (DirectionalVector *)
     protobuf_c_message_unpack (&directional_vector__descriptor,
                                allocator, len, data);
}
void   directional_vector__free_unpacked
                     (DirectionalVector *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &directional_vector__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   bno055__init
                     (BNO055         *message)
{
  static const BNO055 init_value = BNO055__INIT;
  *message = init_value;
}
size_t bno055__get_packed_size
                     (const BNO055 *message)
{
  assert(message->base.descriptor == &bno055__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t bno055__pack
                     (const BNO055 *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &bno055__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t bno055__pack_to_buffer
                     (const BNO055 *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &bno055__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
BNO055 *
       bno055__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (BNO055 *)
     protobuf_c_message_unpack (&bno055__descriptor,
                                allocator, len, data);
}
void   bno055__free_unpacked
                     (BNO055 *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &bno055__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor directional_vector__field_descriptors[3] =
{
  {
    "x",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(DirectionalVector, x),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "y",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(DirectionalVector, y),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "z",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(DirectionalVector, z),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned directional_vector__field_indices_by_name[] = {
  0,   /* field[0] = x */
  1,   /* field[1] = y */
  2,   /* field[2] = z */
};
static const ProtobufCIntRange directional_vector__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor directional_vector__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "DirectionalVector",
  "DirectionalVector",
  "DirectionalVector",
  "",
  sizeof(DirectionalVector),
  3,
  directional_vector__field_descriptors,
  directional_vector__field_indices_by_name,
  1,  directional_vector__number_ranges,
  (ProtobufCMessageInit) directional_vector__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor bno055__field_descriptors[1] =
{
  {
    "direction",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(BNO055, direction),
    &directional_vector__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned bno055__field_indices_by_name[] = {
  0,   /* field[0] = direction */
};
static const ProtobufCIntRange bno055__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor bno055__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "BNO055",
  "BNO055",
  "BNO055",
  "",
  sizeof(BNO055),
  1,
  bno055__field_descriptors,
  bno055__field_indices_by_name,
  1,  bno055__number_ranges,
  (ProtobufCMessageInit) bno055__init,
  NULL,NULL,NULL    /* reserved[123] */
};
