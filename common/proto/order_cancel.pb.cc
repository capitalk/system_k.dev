// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: order_cancel.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "order_cancel.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace capkproto {

namespace {

const ::google::protobuf::Descriptor* order_cancel_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  order_cancel_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_order_5fcancel_2eproto() {
  protobuf_AddDesc_order_5fcancel_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "order_cancel.proto");
  GOOGLE_CHECK(file != NULL);
  order_cancel_descriptor_ = file->message_type(0);
  static const int order_cancel_offsets_[7] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, orig_cl_order_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, cl_order_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, strategy_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, symbol_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, side_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, order_qty_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, fut_sett_date_),
  };
  order_cancel_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      order_cancel_descriptor_,
      order_cancel::default_instance_,
      order_cancel_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(order_cancel, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(order_cancel));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_order_5fcancel_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    order_cancel_descriptor_, &order_cancel::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_order_5fcancel_2eproto() {
  delete order_cancel::default_instance_;
  delete order_cancel_reflection_;
}

void protobuf_AddDesc_order_5fcancel_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::capkproto::protobuf_AddDesc_capk_5fglobals_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\022order_cancel.proto\022\tcapkproto\032\022capk_gl"
    "obals.proto\"\255\001\n\014order_cancel\022\030\n\020orig_cl_"
    "order_id\030\001 \001(\014\022\023\n\013cl_order_id\030\002 \001(\014\022\023\n\013s"
    "trategy_id\030\003 \001(\014\022\016\n\006symbol\030\004 \001(\t\022\037\n\004side"
    "\030\005 \001(\0162\021.capkproto.side_t\022\021\n\torder_qty\030\006"
    " \001(\001\022\025\n\rfut_sett_date\030\007 \001(\t", 227);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "order_cancel.proto", &protobuf_RegisterTypes);
  order_cancel::default_instance_ = new order_cancel();
  order_cancel::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_order_5fcancel_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_order_5fcancel_2eproto {
  StaticDescriptorInitializer_order_5fcancel_2eproto() {
    protobuf_AddDesc_order_5fcancel_2eproto();
  }
} static_descriptor_initializer_order_5fcancel_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int order_cancel::kOrigClOrderIdFieldNumber;
const int order_cancel::kClOrderIdFieldNumber;
const int order_cancel::kStrategyIdFieldNumber;
const int order_cancel::kSymbolFieldNumber;
const int order_cancel::kSideFieldNumber;
const int order_cancel::kOrderQtyFieldNumber;
const int order_cancel::kFutSettDateFieldNumber;
#endif  // !_MSC_VER

order_cancel::order_cancel()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void order_cancel::InitAsDefaultInstance() {
}

order_cancel::order_cancel(const order_cancel& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void order_cancel::SharedCtor() {
  _cached_size_ = 0;
  orig_cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  strategy_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  symbol_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  side_ = 1;
  order_qty_ = 0;
  fut_sett_date_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

order_cancel::~order_cancel() {
  SharedDtor();
}

void order_cancel::SharedDtor() {
  if (orig_cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    delete orig_cl_order_id_;
  }
  if (cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    delete cl_order_id_;
  }
  if (strategy_id_ != &::google::protobuf::internal::kEmptyString) {
    delete strategy_id_;
  }
  if (symbol_ != &::google::protobuf::internal::kEmptyString) {
    delete symbol_;
  }
  if (fut_sett_date_ != &::google::protobuf::internal::kEmptyString) {
    delete fut_sett_date_;
  }
  if (this != default_instance_) {
  }
}

void order_cancel::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* order_cancel::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return order_cancel_descriptor_;
}

const order_cancel& order_cancel::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_order_5fcancel_2eproto();
  return *default_instance_;
}

order_cancel* order_cancel::default_instance_ = NULL;

order_cancel* order_cancel::New() const {
  return new order_cancel;
}

void order_cancel::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_orig_cl_order_id()) {
      if (orig_cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
        orig_cl_order_id_->clear();
      }
    }
    if (has_cl_order_id()) {
      if (cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
        cl_order_id_->clear();
      }
    }
    if (has_strategy_id()) {
      if (strategy_id_ != &::google::protobuf::internal::kEmptyString) {
        strategy_id_->clear();
      }
    }
    if (has_symbol()) {
      if (symbol_ != &::google::protobuf::internal::kEmptyString) {
        symbol_->clear();
      }
    }
    side_ = 1;
    order_qty_ = 0;
    if (has_fut_sett_date()) {
      if (fut_sett_date_ != &::google::protobuf::internal::kEmptyString) {
        fut_sett_date_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool order_cancel::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional bytes orig_cl_order_id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_orig_cl_order_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_cl_order_id;
        break;
      }

      // optional bytes cl_order_id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_cl_order_id:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_cl_order_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_strategy_id;
        break;
      }

      // optional bytes strategy_id = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_strategy_id:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_strategy_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_symbol;
        break;
      }

      // optional string symbol = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_symbol:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_symbol()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->symbol().data(), this->symbol().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_side;
        break;
      }

      // optional .capkproto.side_t side = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_side:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::capkproto::side_t_IsValid(value)) {
            set_side(static_cast< ::capkproto::side_t >(value));
          } else {
            mutable_unknown_fields()->AddVarint(5, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(49)) goto parse_order_qty;
        break;
      }

      // optional double order_qty = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_order_qty:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &order_qty_)));
          set_has_order_qty();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(58)) goto parse_fut_sett_date;
        break;
      }

      // optional string fut_sett_date = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_fut_sett_date:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_fut_sett_date()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->fut_sett_date().data(), this->fut_sett_date().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void order_cancel::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional bytes orig_cl_order_id = 1;
  if (has_orig_cl_order_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->orig_cl_order_id(), output);
  }

  // optional bytes cl_order_id = 2;
  if (has_cl_order_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->cl_order_id(), output);
  }

  // optional bytes strategy_id = 3;
  if (has_strategy_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      3, this->strategy_id(), output);
  }

  // optional string symbol = 4;
  if (has_symbol()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->symbol().data(), this->symbol().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->symbol(), output);
  }

  // optional .capkproto.side_t side = 5;
  if (has_side()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      5, this->side(), output);
  }

  // optional double order_qty = 6;
  if (has_order_qty()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(6, this->order_qty(), output);
  }

  // optional string fut_sett_date = 7;
  if (has_fut_sett_date()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->fut_sett_date().data(), this->fut_sett_date().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      7, this->fut_sett_date(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* order_cancel::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional bytes orig_cl_order_id = 1;
  if (has_orig_cl_order_id()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->orig_cl_order_id(), target);
  }

  // optional bytes cl_order_id = 2;
  if (has_cl_order_id()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->cl_order_id(), target);
  }

  // optional bytes strategy_id = 3;
  if (has_strategy_id()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        3, this->strategy_id(), target);
  }

  // optional string symbol = 4;
  if (has_symbol()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->symbol().data(), this->symbol().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->symbol(), target);
  }

  // optional .capkproto.side_t side = 5;
  if (has_side()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      5, this->side(), target);
  }

  // optional double order_qty = 6;
  if (has_order_qty()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(6, this->order_qty(), target);
  }

  // optional string fut_sett_date = 7;
  if (has_fut_sett_date()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->fut_sett_date().data(), this->fut_sett_date().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        7, this->fut_sett_date(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int order_cancel::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional bytes orig_cl_order_id = 1;
    if (has_orig_cl_order_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->orig_cl_order_id());
    }

    // optional bytes cl_order_id = 2;
    if (has_cl_order_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->cl_order_id());
    }

    // optional bytes strategy_id = 3;
    if (has_strategy_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->strategy_id());
    }

    // optional string symbol = 4;
    if (has_symbol()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->symbol());
    }

    // optional .capkproto.side_t side = 5;
    if (has_side()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->side());
    }

    // optional double order_qty = 6;
    if (has_order_qty()) {
      total_size += 1 + 8;
    }

    // optional string fut_sett_date = 7;
    if (has_fut_sett_date()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->fut_sett_date());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void order_cancel::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const order_cancel* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const order_cancel*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void order_cancel::MergeFrom(const order_cancel& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_orig_cl_order_id()) {
      set_orig_cl_order_id(from.orig_cl_order_id());
    }
    if (from.has_cl_order_id()) {
      set_cl_order_id(from.cl_order_id());
    }
    if (from.has_strategy_id()) {
      set_strategy_id(from.strategy_id());
    }
    if (from.has_symbol()) {
      set_symbol(from.symbol());
    }
    if (from.has_side()) {
      set_side(from.side());
    }
    if (from.has_order_qty()) {
      set_order_qty(from.order_qty());
    }
    if (from.has_fut_sett_date()) {
      set_fut_sett_date(from.fut_sett_date());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void order_cancel::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void order_cancel::CopyFrom(const order_cancel& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool order_cancel::IsInitialized() const {

  return true;
}

void order_cancel::Swap(order_cancel* other) {
  if (other != this) {
    std::swap(orig_cl_order_id_, other->orig_cl_order_id_);
    std::swap(cl_order_id_, other->cl_order_id_);
    std::swap(strategy_id_, other->strategy_id_);
    std::swap(symbol_, other->symbol_);
    std::swap(side_, other->side_);
    std::swap(order_qty_, other->order_qty_);
    std::swap(fut_sett_date_, other->fut_sett_date_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata order_cancel::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = order_cancel_descriptor_;
  metadata.reflection = order_cancel_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace capkproto

// @@protoc_insertion_point(global_scope)
