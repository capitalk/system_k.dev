// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: heartbeat.proto

#ifndef PROTOBUF_heartbeat_2eproto__INCLUDED
#define PROTOBUF_heartbeat_2eproto__INCLUDED

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
void  protobuf_AddDesc_heartbeat_2eproto();
void protobuf_AssignDesc_heartbeat_2eproto();
void protobuf_ShutdownFile_heartbeat_2eproto();

class hearbeat;

// ===================================================================

class hearbeat : public ::google::protobuf::Message {
 public:
  hearbeat();
  virtual ~hearbeat();

  hearbeat(const hearbeat& from);

  inline hearbeat& operator=(const hearbeat& from) {
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
  static const hearbeat& default_instance();

  void Swap(hearbeat* other);

  // implements Message ----------------------------------------------

  hearbeat* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const hearbeat& from);
  void MergeFrom(const hearbeat& from);
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

  // required sfixed32 venue_id = 1;
  inline bool has_venue_id() const;
  inline void clear_venue_id();
  static const int kVenueIdFieldNumber = 1;
  inline ::google::protobuf::int32 venue_id() const;
  inline void set_venue_id(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:capkproto.hearbeat)
 private:
  inline void set_has_venue_id();
  inline void clear_has_venue_id();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 venue_id_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_heartbeat_2eproto();
  friend void protobuf_AssignDesc_heartbeat_2eproto();
  friend void protobuf_ShutdownFile_heartbeat_2eproto();

  void InitAsDefaultInstance();
  static hearbeat* default_instance_;
};
// ===================================================================


// ===================================================================

// hearbeat

// required sfixed32 venue_id = 1;
inline bool hearbeat::has_venue_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void hearbeat::set_has_venue_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void hearbeat::clear_has_venue_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void hearbeat::clear_venue_id() {
  venue_id_ = 0;
  clear_has_venue_id();
}
inline ::google::protobuf::int32 hearbeat::venue_id() const {
  return venue_id_;
}
inline void hearbeat::set_venue_id(::google::protobuf::int32 value) {
  set_has_venue_id();
  venue_id_ = value;
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

#endif  // PROTOBUF_heartbeat_2eproto__INCLUDED