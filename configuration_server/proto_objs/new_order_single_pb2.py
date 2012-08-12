# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


import capk_globals_pb2

DESCRIPTOR = descriptor.FileDescriptor(
  name='new_order_single.proto',
  package='capkproto',
  serialized_pb='\n\x16new_order_single.proto\x12\tcapkproto\x1a\x12\x63\x61pk_globals.proto\"\xd8\x01\n\x10new_order_single\x12\x10\n\x08order_id\x18\x01 \x01(\x0c\x12\x13\n\x0bstrategy_id\x18\x02 \x01(\x0c\x12\x0e\n\x06symbol\x18\x03 \x01(\t\x12\x1f\n\x04side\x18\x04 \x01(\x0e\x32\x11.capkproto.side_t\x12\x11\n\torder_qty\x18\x05 \x01(\x01\x12\x10\n\x08ord_type\x18\x06 \x01(\x0f\x12\r\n\x05price\x18\x07 \x01(\x01\x12\x15\n\rtime_in_force\x18\x08 \x01(\x0f\x12\x0f\n\x07\x61\x63\x63ount\x18\t \x01(\t\x12\x10\n\x08venue_id\x18\n \x01(\x0f')




_NEW_ORDER_SINGLE = descriptor.Descriptor(
  name='new_order_single',
  full_name='capkproto.new_order_single',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='order_id', full_name='capkproto.new_order_single.order_id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='strategy_id', full_name='capkproto.new_order_single.strategy_id', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='symbol', full_name='capkproto.new_order_single.symbol', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='side', full_name='capkproto.new_order_single.side', index=3,
      number=4, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='order_qty', full_name='capkproto.new_order_single.order_qty', index=4,
      number=5, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ord_type', full_name='capkproto.new_order_single.ord_type', index=5,
      number=6, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='price', full_name='capkproto.new_order_single.price', index=6,
      number=7, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='time_in_force', full_name='capkproto.new_order_single.time_in_force', index=7,
      number=8, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='account', full_name='capkproto.new_order_single.account', index=8,
      number=9, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='venue_id', full_name='capkproto.new_order_single.venue_id', index=9,
      number=10, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=58,
  serialized_end=274,
)

_NEW_ORDER_SINGLE.fields_by_name['side'].enum_type = capk_globals_pb2._SIDE_T
DESCRIPTOR.message_types_by_name['new_order_single'] = _NEW_ORDER_SINGLE

class new_order_single(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _NEW_ORDER_SINGLE
  
  # @@protoc_insertion_point(class_scope:capkproto.new_order_single)

# @@protoc_insertion_point(module_scope)
