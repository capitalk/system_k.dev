# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: spot_fx_md_1.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


import capk_globals_pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='spot_fx_md_1.proto',
  package='capkproto',
  serialized_pb='\n\x12spot_fx_md_1.proto\x12\tcapkproto\x1a\x12\x63\x61pk_globals.proto\"\xc2\x01\n\x07order_t\x12\x0c\n\x04name\x18\x01 \x02(\t\x12\x0b\n\x03mic\x18\x02 \x02(\t\x12\x0b\n\x03qid\x18\x03 \x02(\x0f\x12\'\n\x04side\x18\x04 \x02(\x0e\x32\x19.capkproto.order_t.side_t\x12\x0c\n\x04size\x18\x05 \x02(\x01\x12\r\n\x05price\x18\x06 \x02(\x01\"\x1a\n\x06side_t\x12\x07\n\x03\x42ID\x10\x00\x12\x07\n\x03\x41SK\x10\x01\"-\n\x0cquote_type_t\x12\x0e\n\nINDICATIVE\x10\x00\x12\r\n\tTRADEABLE\x10\x01\"\x7f\n\x11single_market_bbo\x12\x10\n\x08venue_id\x18\x01 \x02(\x0f\x12\x0e\n\x06symbol\x18\x02 \x02(\t\x12\x10\n\x08\x62id_size\x18\x03 \x02(\x01\x12\x11\n\tbid_price\x18\x04 \x02(\x01\x12\x10\n\x08\x61sk_size\x18\x05 \x02(\x01\x12\x11\n\task_price\x18\x06 \x02(\x01\"\xa8\x01\n\x0einstrument_bbo\x12\x0e\n\x06symbol\x18\x01 \x02(\t\x12\x14\n\x0c\x62id_venue_id\x18\x02 \x02(\x0f\x12\x11\n\tbid_price\x18\x03 \x02(\x01\x12\x10\n\x08\x62id_size\x18\x04 \x02(\x01\x12\x14\n\x0c\x61sk_venue_id\x18\x05 \x02(\x0f\x12\x11\n\task_price\x18\x06 \x02(\x01\x12\x10\n\x08\x61sk_size\x18\x07 \x02(\x01\x12\x10\n\x08sequence\x18\x08 \x01(\x0f')



_ORDER_T_SIDE_T = _descriptor.EnumDescriptor(
  name='side_t',
  full_name='capkproto.order_t.side_t',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='BID', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ASK', index=1, number=1,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=175,
  serialized_end=201,
)

_ORDER_T_QUOTE_TYPE_T = _descriptor.EnumDescriptor(
  name='quote_type_t',
  full_name='capkproto.order_t.quote_type_t',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='INDICATIVE', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='TRADEABLE', index=1, number=1,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=203,
  serialized_end=248,
)


_ORDER_T = _descriptor.Descriptor(
  name='order_t',
  full_name='capkproto.order_t',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='capkproto.order_t.name', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='mic', full_name='capkproto.order_t.mic', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='qid', full_name='capkproto.order_t.qid', index=2,
      number=3, type=15, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='side', full_name='capkproto.order_t.side', index=3,
      number=4, type=14, cpp_type=8, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='size', full_name='capkproto.order_t.size', index=4,
      number=5, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='price', full_name='capkproto.order_t.price', index=5,
      number=6, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _ORDER_T_SIDE_T,
    _ORDER_T_QUOTE_TYPE_T,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=54,
  serialized_end=248,
)


_SINGLE_MARKET_BBO = _descriptor.Descriptor(
  name='single_market_bbo',
  full_name='capkproto.single_market_bbo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='venue_id', full_name='capkproto.single_market_bbo.venue_id', index=0,
      number=1, type=15, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='symbol', full_name='capkproto.single_market_bbo.symbol', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='bid_size', full_name='capkproto.single_market_bbo.bid_size', index=2,
      number=3, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='bid_price', full_name='capkproto.single_market_bbo.bid_price', index=3,
      number=4, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ask_size', full_name='capkproto.single_market_bbo.ask_size', index=4,
      number=5, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ask_price', full_name='capkproto.single_market_bbo.ask_price', index=5,
      number=6, type=1, cpp_type=5, label=2,
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
  serialized_start=250,
  serialized_end=377,
)


_INSTRUMENT_BBO = _descriptor.Descriptor(
  name='instrument_bbo',
  full_name='capkproto.instrument_bbo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='symbol', full_name='capkproto.instrument_bbo.symbol', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='bid_venue_id', full_name='capkproto.instrument_bbo.bid_venue_id', index=1,
      number=2, type=15, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='bid_price', full_name='capkproto.instrument_bbo.bid_price', index=2,
      number=3, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='bid_size', full_name='capkproto.instrument_bbo.bid_size', index=3,
      number=4, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ask_venue_id', full_name='capkproto.instrument_bbo.ask_venue_id', index=4,
      number=5, type=15, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ask_price', full_name='capkproto.instrument_bbo.ask_price', index=5,
      number=6, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ask_size', full_name='capkproto.instrument_bbo.ask_size', index=6,
      number=7, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sequence', full_name='capkproto.instrument_bbo.sequence', index=7,
      number=8, type=15, cpp_type=1, label=1,
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
  serialized_start=380,
  serialized_end=548,
)

_ORDER_T.fields_by_name['side'].enum_type = _ORDER_T_SIDE_T
_ORDER_T_SIDE_T.containing_type = _ORDER_T;
_ORDER_T_QUOTE_TYPE_T.containing_type = _ORDER_T;
DESCRIPTOR.message_types_by_name['order_t'] = _ORDER_T
DESCRIPTOR.message_types_by_name['single_market_bbo'] = _SINGLE_MARKET_BBO
DESCRIPTOR.message_types_by_name['instrument_bbo'] = _INSTRUMENT_BBO

class order_t(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ORDER_T

  # @@protoc_insertion_point(class_scope:capkproto.order_t)

class single_market_bbo(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SINGLE_MARKET_BBO

  # @@protoc_insertion_point(class_scope:capkproto.single_market_bbo)

class instrument_bbo(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _INSTRUMENT_BBO

  # @@protoc_insertion_point(class_scope:capkproto.instrument_bbo)


# @@protoc_insertion_point(module_scope)
