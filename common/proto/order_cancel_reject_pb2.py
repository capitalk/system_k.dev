# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: order_cancel_reject.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


import capk_globals_pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='order_cancel_reject.proto',
  package='capkproto',
  serialized_pb='\n\x19order_cancel_reject.proto\x12\tcapkproto\x1a\x12\x63\x61pk_globals.proto\"\xcc\x01\n\x13order_cancel_reject\x12\x18\n\x10orig_cl_order_id\x18\x01 \x01(\x0c\x12\x13\n\x0b\x63l_order_id\x18\x02 \x01(\x0c\x12\x13\n\x0bstrategy_id\x18\x03 \x01(\x0c\x12\x14\n\x0corder_status\x18\x04 \x01(\x0f\x12\x1c\n\x14\x63\x61ncel_reject_reason\x18\x05 \x01(\x0f\x12!\n\x19\x63\x61ncel_reject_response_to\x18\x06 \x01(\x0f\x12\x1a\n\x12\x63\x61ncel_reject_text\x18\x07 \x01(\t')




_ORDER_CANCEL_REJECT = _descriptor.Descriptor(
  name='order_cancel_reject',
  full_name='capkproto.order_cancel_reject',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='orig_cl_order_id', full_name='capkproto.order_cancel_reject.orig_cl_order_id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cl_order_id', full_name='capkproto.order_cancel_reject.cl_order_id', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='strategy_id', full_name='capkproto.order_cancel_reject.strategy_id', index=2,
      number=3, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='order_status', full_name='capkproto.order_cancel_reject.order_status', index=3,
      number=4, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cancel_reject_reason', full_name='capkproto.order_cancel_reject.cancel_reject_reason', index=4,
      number=5, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cancel_reject_response_to', full_name='capkproto.order_cancel_reject.cancel_reject_response_to', index=5,
      number=6, type=15, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cancel_reject_text', full_name='capkproto.order_cancel_reject.cancel_reject_text', index=6,
      number=7, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
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
  serialized_end=265,
)

DESCRIPTOR.message_types_by_name['order_cancel_reject'] = _ORDER_CANCEL_REJECT

class order_cancel_reject(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ORDER_CANCEL_REJECT

  # @@protoc_insertion_point(class_scope:capkproto.order_cancel_reject)


# @@protoc_insertion_point(module_scope)
