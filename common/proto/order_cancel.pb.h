// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: order_cancel.proto

#ifndef PROTOBUF_order_5fcancel_2eproto__INCLUDED
#define PROTOBUF_order_5fcancel_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
#include "capk_globals.pb.h"
// @@protoc_insertion_point(includes)

namespace capkproto {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_order_5fcancel_2eproto();
void protobuf_AssignDesc_order_5fcancel_2eproto();
void protobuf_ShutdownFile_order_5fcancel_2eproto();

class order_cancel;

// ===================================================================

class order_cancel : public ::google::protobuf::Message {
 public:
  order_cancel();
  virtual ~order_cancel();

  order_cancel(const order_cancel& from);

  inline order_cancel& operator=(const order_cancel& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const order_cancel& default_instance();

  void Swap(order_cancel* other);

  // implements Message ----------------------------------------------

  order_cancel* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const order_cancel& from);
  void MergeFrom(const order_cancel& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bytes orig_cl_order_id = 1;
  inline bool has_orig_cl_order_id() const;
  inline void clear_orig_cl_order_id();
  static const int kOrigClOrderIdFieldNumber = 1;
  inline const ::std::string& orig_cl_order_id() const;
  inline void set_orig_cl_order_id(const ::std::string& value);
  inline void set_orig_cl_order_id(const char* value);
  inline void set_orig_cl_order_id(const void* value, size_t size);
  inline ::std::string* mutable_orig_cl_order_id();
  inline ::std::string* release_orig_cl_order_id();
  inline void set_allocated_orig_cl_order_id(::std::string* orig_cl_order_id);

  // optional bytes cl_order_id = 2;
  inline bool has_cl_order_id() const;
  inline void clear_cl_order_id();
  static const int kClOrderIdFieldNumber = 2;
  inline const ::std::string& cl_order_id() const;
  inline void set_cl_order_id(const ::std::string& value);
  inline void set_cl_order_id(const char* value);
  inline void set_cl_order_id(const void* value, size_t size);
  inline ::std::string* mutable_cl_order_id();
  inline ::std::string* release_cl_order_id();
  inline void set_allocated_cl_order_id(::std::string* cl_order_id);

  // optional bytes strategy_id = 3;
  inline bool has_strategy_id() const;
  inline void clear_strategy_id();
  static const int kStrategyIdFieldNumber = 3;
  inline const ::std::string& strategy_id() const;
  inline void set_strategy_id(const ::std::string& value);
  inline void set_strategy_id(const char* value);
  inline void set_strategy_id(const void* value, size_t size);
  inline ::std::string* mutable_strategy_id();
  inline ::std::string* release_strategy_id();
  inline void set_allocated_strategy_id(::std::string* strategy_id);

  // optional string symbol = 4;
  inline bool has_symbol() const;
  inline void clear_symbol();
  static const int kSymbolFieldNumber = 4;
  inline const ::std::string& symbol() const;
  inline void set_symbol(const ::std::string& value);
  inline void set_symbol(const char* value);
  inline void set_symbol(const char* value, size_t size);
  inline ::std::string* mutable_symbol();
  inline ::std::string* release_symbol();
  inline void set_allocated_symbol(::std::string* symbol);

  // optional .capkproto.side_t side = 5;
  inline bool has_side() const;
  inline void clear_side();
  static const int kSideFieldNumber = 5;
  inline ::capkproto::side_t side() const;
  inline void set_side(::capkproto::side_t value);

  // optional double order_qty = 6;
  inline bool has_order_qty() const;
  inline void clear_order_qty();
  static const int kOrderQtyFieldNumber = 6;
  inline double order_qty() const;
  inline void set_order_qty(double value);

  // optional string fut_sett_date = 7;
  inline bool has_fut_sett_date() const;
  inline void clear_fut_sett_date();
  static const int kFutSettDateFieldNumber = 7;
  inline const ::std::string& fut_sett_date() const;
  inline void set_fut_sett_date(const ::std::string& value);
  inline void set_fut_sett_date(const char* value);
  inline void set_fut_sett_date(const char* value, size_t size);
  inline ::std::string* mutable_fut_sett_date();
  inline ::std::string* release_fut_sett_date();
  inline void set_allocated_fut_sett_date(::std::string* fut_sett_date);

  // @@protoc_insertion_point(class_scope:capkproto.order_cancel)
 private:
  inline void set_has_orig_cl_order_id();
  inline void clear_has_orig_cl_order_id();
  inline void set_has_cl_order_id();
  inline void clear_has_cl_order_id();
  inline void set_has_strategy_id();
  inline void clear_has_strategy_id();
  inline void set_has_symbol();
  inline void clear_has_symbol();
  inline void set_has_side();
  inline void clear_has_side();
  inline void set_has_order_qty();
  inline void clear_has_order_qty();
  inline void set_has_fut_sett_date();
  inline void clear_has_fut_sett_date();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* orig_cl_order_id_;
  ::std::string* cl_order_id_;
  ::std::string* strategy_id_;
  ::std::string* symbol_;
  double order_qty_;
  ::std::string* fut_sett_date_;
  int side_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];

  friend void  protobuf_AddDesc_order_5fcancel_2eproto();
  friend void protobuf_AssignDesc_order_5fcancel_2eproto();
  friend void protobuf_ShutdownFile_order_5fcancel_2eproto();

  void InitAsDefaultInstance();
  static order_cancel* default_instance_;
};
// ===================================================================


// ===================================================================

// order_cancel

// optional bytes orig_cl_order_id = 1;
inline bool order_cancel::has_orig_cl_order_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void order_cancel::set_has_orig_cl_order_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void order_cancel::clear_has_orig_cl_order_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void order_cancel::clear_orig_cl_order_id() {
  if (orig_cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    orig_cl_order_id_->clear();
  }
  clear_has_orig_cl_order_id();
}
inline const ::std::string& order_cancel::orig_cl_order_id() const {
  return *orig_cl_order_id_;
}
inline void order_cancel::set_orig_cl_order_id(const ::std::string& value) {
  set_has_orig_cl_order_id();
  if (orig_cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    orig_cl_order_id_ = new ::std::string;
  }
  orig_cl_order_id_->assign(value);
}
inline void order_cancel::set_orig_cl_order_id(const char* value) {
  set_has_orig_cl_order_id();
  if (orig_cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    orig_cl_order_id_ = new ::std::string;
  }
  orig_cl_order_id_->assign(value);
}
inline void order_cancel::set_orig_cl_order_id(const void* value, size_t size) {
  set_has_orig_cl_order_id();
  if (orig_cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    orig_cl_order_id_ = new ::std::string;
  }
  orig_cl_order_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* order_cancel::mutable_orig_cl_order_id() {
  set_has_orig_cl_order_id();
  if (orig_cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    orig_cl_order_id_ = new ::std::string;
  }
  return orig_cl_order_id_;
}
inline ::std::string* order_cancel::release_orig_cl_order_id() {
  clear_has_orig_cl_order_id();
  if (orig_cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = orig_cl_order_id_;
    orig_cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void order_cancel::set_allocated_orig_cl_order_id(::std::string* orig_cl_order_id) {
  if (orig_cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    delete orig_cl_order_id_;
  }
  if (orig_cl_order_id) {
    set_has_orig_cl_order_id();
    orig_cl_order_id_ = orig_cl_order_id;
  } else {
    clear_has_orig_cl_order_id();
    orig_cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional bytes cl_order_id = 2;
inline bool order_cancel::has_cl_order_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void order_cancel::set_has_cl_order_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void order_cancel::clear_has_cl_order_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void order_cancel::clear_cl_order_id() {
  if (cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    cl_order_id_->clear();
  }
  clear_has_cl_order_id();
}
inline const ::std::string& order_cancel::cl_order_id() const {
  return *cl_order_id_;
}
inline void order_cancel::set_cl_order_id(const ::std::string& value) {
  set_has_cl_order_id();
  if (cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    cl_order_id_ = new ::std::string;
  }
  cl_order_id_->assign(value);
}
inline void order_cancel::set_cl_order_id(const char* value) {
  set_has_cl_order_id();
  if (cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    cl_order_id_ = new ::std::string;
  }
  cl_order_id_->assign(value);
}
inline void order_cancel::set_cl_order_id(const void* value, size_t size) {
  set_has_cl_order_id();
  if (cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    cl_order_id_ = new ::std::string;
  }
  cl_order_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* order_cancel::mutable_cl_order_id() {
  set_has_cl_order_id();
  if (cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    cl_order_id_ = new ::std::string;
  }
  return cl_order_id_;
}
inline ::std::string* order_cancel::release_cl_order_id() {
  clear_has_cl_order_id();
  if (cl_order_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = cl_order_id_;
    cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void order_cancel::set_allocated_cl_order_id(::std::string* cl_order_id) {
  if (cl_order_id_ != &::google::protobuf::internal::kEmptyString) {
    delete cl_order_id_;
  }
  if (cl_order_id) {
    set_has_cl_order_id();
    cl_order_id_ = cl_order_id;
  } else {
    clear_has_cl_order_id();
    cl_order_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional bytes strategy_id = 3;
inline bool order_cancel::has_strategy_id() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void order_cancel::set_has_strategy_id() {
  _has_bits_[0] |= 0x00000004u;
}
inline void order_cancel::clear_has_strategy_id() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void order_cancel::clear_strategy_id() {
  if (strategy_id_ != &::google::protobuf::internal::kEmptyString) {
    strategy_id_->clear();
  }
  clear_has_strategy_id();
}
inline const ::std::string& order_cancel::strategy_id() const {
  return *strategy_id_;
}
inline void order_cancel::set_strategy_id(const ::std::string& value) {
  set_has_strategy_id();
  if (strategy_id_ == &::google::protobuf::internal::kEmptyString) {
    strategy_id_ = new ::std::string;
  }
  strategy_id_->assign(value);
}
inline void order_cancel::set_strategy_id(const char* value) {
  set_has_strategy_id();
  if (strategy_id_ == &::google::protobuf::internal::kEmptyString) {
    strategy_id_ = new ::std::string;
  }
  strategy_id_->assign(value);
}
inline void order_cancel::set_strategy_id(const void* value, size_t size) {
  set_has_strategy_id();
  if (strategy_id_ == &::google::protobuf::internal::kEmptyString) {
    strategy_id_ = new ::std::string;
  }
  strategy_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* order_cancel::mutable_strategy_id() {
  set_has_strategy_id();
  if (strategy_id_ == &::google::protobuf::internal::kEmptyString) {
    strategy_id_ = new ::std::string;
  }
  return strategy_id_;
}
inline ::std::string* order_cancel::release_strategy_id() {
  clear_has_strategy_id();
  if (strategy_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = strategy_id_;
    strategy_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void order_cancel::set_allocated_strategy_id(::std::string* strategy_id) {
  if (strategy_id_ != &::google::protobuf::internal::kEmptyString) {
    delete strategy_id_;
  }
  if (strategy_id) {
    set_has_strategy_id();
    strategy_id_ = strategy_id;
  } else {
    clear_has_strategy_id();
    strategy_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string symbol = 4;
inline bool order_cancel::has_symbol() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void order_cancel::set_has_symbol() {
  _has_bits_[0] |= 0x00000008u;
}
inline void order_cancel::clear_has_symbol() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void order_cancel::clear_symbol() {
  if (symbol_ != &::google::protobuf::internal::kEmptyString) {
    symbol_->clear();
  }
  clear_has_symbol();
}
inline const ::std::string& order_cancel::symbol() const {
  return *symbol_;
}
inline void order_cancel::set_symbol(const ::std::string& value) {
  set_has_symbol();
  if (symbol_ == &::google::protobuf::internal::kEmptyString) {
    symbol_ = new ::std::string;
  }
  symbol_->assign(value);
}
inline void order_cancel::set_symbol(const char* value) {
  set_has_symbol();
  if (symbol_ == &::google::protobuf::internal::kEmptyString) {
    symbol_ = new ::std::string;
  }
  symbol_->assign(value);
}
inline void order_cancel::set_symbol(const char* value, size_t size) {
  set_has_symbol();
  if (symbol_ == &::google::protobuf::internal::kEmptyString) {
    symbol_ = new ::std::string;
  }
  symbol_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* order_cancel::mutable_symbol() {
  set_has_symbol();
  if (symbol_ == &::google::protobuf::internal::kEmptyString) {
    symbol_ = new ::std::string;
  }
  return symbol_;
}
inline ::std::string* order_cancel::release_symbol() {
  clear_has_symbol();
  if (symbol_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = symbol_;
    symbol_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void order_cancel::set_allocated_symbol(::std::string* symbol) {
  if (symbol_ != &::google::protobuf::internal::kEmptyString) {
    delete symbol_;
  }
  if (symbol) {
    set_has_symbol();
    symbol_ = symbol;
  } else {
    clear_has_symbol();
    symbol_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional .capkproto.side_t side = 5;
inline bool order_cancel::has_side() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void order_cancel::set_has_side() {
  _has_bits_[0] |= 0x00000010u;
}
inline void order_cancel::clear_has_side() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void order_cancel::clear_side() {
  side_ = 1;
  clear_has_side();
}
inline ::capkproto::side_t order_cancel::side() const {
  return static_cast< ::capkproto::side_t >(side_);
}
inline void order_cancel::set_side(::capkproto::side_t value) {
  assert(::capkproto::side_t_IsValid(value));
  set_has_side();
  side_ = value;
}

// optional double order_qty = 6;
inline bool order_cancel::has_order_qty() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void order_cancel::set_has_order_qty() {
  _has_bits_[0] |= 0x00000020u;
}
inline void order_cancel::clear_has_order_qty() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void order_cancel::clear_order_qty() {
  order_qty_ = 0;
  clear_has_order_qty();
}
inline double order_cancel::order_qty() const {
  return order_qty_;
}
inline void order_cancel::set_order_qty(double value) {
  set_has_order_qty();
  order_qty_ = value;
}

// optional string fut_sett_date = 7;
inline bool order_cancel::has_fut_sett_date() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void order_cancel::set_has_fut_sett_date() {
  _has_bits_[0] |= 0x00000040u;
}
inline void order_cancel::clear_has_fut_sett_date() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void order_cancel::clear_fut_sett_date() {
  if (fut_sett_date_ != &::google::protobuf::internal::kEmptyString) {
    fut_sett_date_->clear();
  }
  clear_has_fut_sett_date();
}
inline const ::std::string& order_cancel::fut_sett_date() const {
  return *fut_sett_date_;
}
inline void order_cancel::set_fut_sett_date(const ::std::string& value) {
  set_has_fut_sett_date();
  if (fut_sett_date_ == &::google::protobuf::internal::kEmptyString) {
    fut_sett_date_ = new ::std::string;
  }
  fut_sett_date_->assign(value);
}
inline void order_cancel::set_fut_sett_date(const char* value) {
  set_has_fut_sett_date();
  if (fut_sett_date_ == &::google::protobuf::internal::kEmptyString) {
    fut_sett_date_ = new ::std::string;
  }
  fut_sett_date_->assign(value);
}
inline void order_cancel::set_fut_sett_date(const char* value, size_t size) {
  set_has_fut_sett_date();
  if (fut_sett_date_ == &::google::protobuf::internal::kEmptyString) {
    fut_sett_date_ = new ::std::string;
  }
  fut_sett_date_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* order_cancel::mutable_fut_sett_date() {
  set_has_fut_sett_date();
  if (fut_sett_date_ == &::google::protobuf::internal::kEmptyString) {
    fut_sett_date_ = new ::std::string;
  }
  return fut_sett_date_;
}
inline ::std::string* order_cancel::release_fut_sett_date() {
  clear_has_fut_sett_date();
  if (fut_sett_date_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = fut_sett_date_;
    fut_sett_date_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void order_cancel::set_allocated_fut_sett_date(::std::string* fut_sett_date) {
  if (fut_sett_date_ != &::google::protobuf::internal::kEmptyString) {
    delete fut_sett_date_;
  }
  if (fut_sett_date) {
    set_has_fut_sett_date();
    fut_sett_date_ = fut_sett_date;
  } else {
    clear_has_fut_sett_date();
    fut_sett_date_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace capkproto

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_order_5fcancel_2eproto__INCLUDED
