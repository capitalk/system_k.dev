# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: venue_configuration.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


import capk_globals_pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='venue_configuration.proto',
  package='capkproto',
  serialized_pb='\n\x19venue_configuration.proto\x12\tcapkproto\x1a\x12\x63\x61pk_globals.proto\"\xda\x01\n\x13venue_configuration\x12\x10\n\x08venue_id\x18\x01 \x01(\x0f\x12\x10\n\x08mic_name\x18\x02 \x01(\t\x12\x1c\n\x14order_interface_addr\x18\x03 \x01(\t\x12\x17\n\x0forder_ping_addr\x18\x04 \x01(\t\x12\"\n\x1amarket_data_broadcast_addr\x18\x05 \x01(\t\x12\x1e\n\x16logging_broadcast_addr\x18\x06 \x01(\t\x12$\n\x1cuse_synthetic_cancel_replace\x18\x07 \x01(\x08\"\xc4\x01\n\rconfiguration\x12 \n\x18trade_serialization_addr\x18\x01 \x01(\t\x12\x1e\n\x16recovery_listener_addr\x18\x02 \x01(\t\x12 \n\x18\x61ggregated_bbo_book_addr\x18\x03 \x01(\t\x12\x1e\n\x16\x61ggregated_bbo_book_id\x18\x04 \x01(\x0f\x12/\n\x07\x63onfigs\x18\x05 \x03(\x0b\x32\x1e.capkproto.venue_configuration')




_VENUE_CONFIGURATION = _descriptor.Descriptor(
  name='venue_configuration',
  full_name='capkproto.venue_configuration',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='venue_id', full_name='capkproto.venue_configuration.venue_id', index=0,
      number=1, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='mic_name', full_name='capkproto.venue_configuration.mic_name', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='order_interface_addr', full_name='capkproto.venue_configuration.order_interface_addr', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='order_ping_addr', full_name='capkproto.venue_configuration.order_ping_addr', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='market_data_broadcast_addr', full_name='capkproto.venue_configuration.market_data_broadcast_addr', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='logging_broadcast_addr', full_name='capkproto.venue_configuration.logging_broadcast_addr', index=5,
      number=6, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='use_synthetic_cancel_replace', full_name='capkproto.venue_configuration.use_synthetic_cancel_replace', index=6,
      number=7, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
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
  serialized_start=61,
  serialized_end=279,
)


_CONFIGURATION = _descriptor.Descriptor(
  name='configuration',
  full_name='capkproto.configuration',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='trade_serialization_addr', full_name='capkproto.configuration.trade_serialization_addr', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='recovery_listener_addr', full_name='capkproto.configuration.recovery_listener_addr', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='aggregated_bbo_book_addr', full_name='capkproto.configuration.aggregated_bbo_book_addr', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='aggregated_bbo_book_id', full_name='capkproto.configuration.aggregated_bbo_book_id', index=3,
      number=4, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='configs', full_name='capkproto.configuration.configs', index=4,
      number=5, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
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
  serialized_start=282,
  serialized_end=478,
)

_CONFIGURATION.fields_by_name['configs'].message_type = _VENUE_CONFIGURATION
DESCRIPTOR.message_types_by_name['venue_configuration'] = _VENUE_CONFIGURATION
DESCRIPTOR.message_types_by_name['configuration'] = _CONFIGURATION

class venue_configuration(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _VENUE_CONFIGURATION

  # @@protoc_insertion_point(class_scope:capkproto.venue_configuration)

class configuration(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _CONFIGURATION

  # @@protoc_insertion_point(class_scope:capkproto.configuration)


# @@protoc_insertion_point(module_scope)
