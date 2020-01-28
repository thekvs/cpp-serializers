// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_test_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_test_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3011000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3011002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_test_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_test_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_test_2eproto;
namespace protobuf_test {
class Record;
class RecordDefaultTypeInternal;
extern RecordDefaultTypeInternal _Record_default_instance_;
}  // namespace protobuf_test
PROTOBUF_NAMESPACE_OPEN
template<> ::protobuf_test::Record* Arena::CreateMaybeMessage<::protobuf_test::Record>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace protobuf_test {

// ===================================================================

class Record :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:protobuf_test.Record) */ {
 public:
  Record();
  virtual ~Record();

  Record(const Record& from);
  Record(Record&& from) noexcept
    : Record() {
    *this = ::std::move(from);
  }

  inline Record& operator=(const Record& from) {
    CopyFrom(from);
    return *this;
  }
  inline Record& operator=(Record&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Record& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Record* internal_default_instance() {
    return reinterpret_cast<const Record*>(
               &_Record_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Record& a, Record& b) {
    a.Swap(&b);
  }
  inline void Swap(Record* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Record* New() const final {
    return CreateMaybeMessage<Record>(nullptr);
  }

  Record* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Record>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Record& from);
  void MergeFrom(const Record& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Record* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "protobuf_test.Record";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_test_2eproto);
    return ::descriptor_table_test_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIdsFieldNumber = 1,
    kStringsFieldNumber = 2,
  };
  // repeated int64 ids = 1;
  int ids_size() const;
  private:
  int _internal_ids_size() const;
  public:
  void clear_ids();
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_ids(int index) const;
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
      _internal_ids() const;
  void _internal_add_ids(::PROTOBUF_NAMESPACE_ID::int64 value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
      _internal_mutable_ids();
  public:
  ::PROTOBUF_NAMESPACE_ID::int64 ids(int index) const;
  void set_ids(int index, ::PROTOBUF_NAMESPACE_ID::int64 value);
  void add_ids(::PROTOBUF_NAMESPACE_ID::int64 value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
      ids() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
      mutable_ids();

  // repeated string strings = 2;
  int strings_size() const;
  private:
  int _internal_strings_size() const;
  public:
  void clear_strings();
  const std::string& strings(int index) const;
  std::string* mutable_strings(int index);
  void set_strings(int index, const std::string& value);
  void set_strings(int index, std::string&& value);
  void set_strings(int index, const char* value);
  void set_strings(int index, const char* value, size_t size);
  std::string* add_strings();
  void add_strings(const std::string& value);
  void add_strings(std::string&& value);
  void add_strings(const char* value);
  void add_strings(const char* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& strings() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_strings();
  private:
  const std::string& _internal_strings(int index) const;
  std::string* _internal_add_strings();
  public:

  // @@protoc_insertion_point(class_scope:protobuf_test.Record)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 > ids_;
  mutable std::atomic<int> _ids_cached_byte_size_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> strings_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_test_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Record

// repeated int64 ids = 1;
inline int Record::_internal_ids_size() const {
  return ids_.size();
}
inline int Record::ids_size() const {
  return _internal_ids_size();
}
inline void Record::clear_ids() {
  ids_.Clear();
}
inline ::PROTOBUF_NAMESPACE_ID::int64 Record::_internal_ids(int index) const {
  return ids_.Get(index);
}
inline ::PROTOBUF_NAMESPACE_ID::int64 Record::ids(int index) const {
  // @@protoc_insertion_point(field_get:protobuf_test.Record.ids)
  return _internal_ids(index);
}
inline void Record::set_ids(int index, ::PROTOBUF_NAMESPACE_ID::int64 value) {
  ids_.Set(index, value);
  // @@protoc_insertion_point(field_set:protobuf_test.Record.ids)
}
inline void Record::_internal_add_ids(::PROTOBUF_NAMESPACE_ID::int64 value) {
  ids_.Add(value);
}
inline void Record::add_ids(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_add_ids(value);
  // @@protoc_insertion_point(field_add:protobuf_test.Record.ids)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
Record::_internal_ids() const {
  return ids_;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >&
Record::ids() const {
  // @@protoc_insertion_point(field_list:protobuf_test.Record.ids)
  return _internal_ids();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
Record::_internal_mutable_ids() {
  return &ids_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< ::PROTOBUF_NAMESPACE_ID::int64 >*
Record::mutable_ids() {
  // @@protoc_insertion_point(field_mutable_list:protobuf_test.Record.ids)
  return _internal_mutable_ids();
}

// repeated string strings = 2;
inline int Record::_internal_strings_size() const {
  return strings_.size();
}
inline int Record::strings_size() const {
  return _internal_strings_size();
}
inline void Record::clear_strings() {
  strings_.Clear();
}
inline std::string* Record::add_strings() {
  // @@protoc_insertion_point(field_add_mutable:protobuf_test.Record.strings)
  return _internal_add_strings();
}
inline const std::string& Record::_internal_strings(int index) const {
  return strings_.Get(index);
}
inline const std::string& Record::strings(int index) const {
  // @@protoc_insertion_point(field_get:protobuf_test.Record.strings)
  return _internal_strings(index);
}
inline std::string* Record::mutable_strings(int index) {
  // @@protoc_insertion_point(field_mutable:protobuf_test.Record.strings)
  return strings_.Mutable(index);
}
inline void Record::set_strings(int index, const std::string& value) {
  // @@protoc_insertion_point(field_set:protobuf_test.Record.strings)
  strings_.Mutable(index)->assign(value);
}
inline void Record::set_strings(int index, std::string&& value) {
  // @@protoc_insertion_point(field_set:protobuf_test.Record.strings)
  strings_.Mutable(index)->assign(std::move(value));
}
inline void Record::set_strings(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  strings_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:protobuf_test.Record.strings)
}
inline void Record::set_strings(int index, const char* value, size_t size) {
  strings_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:protobuf_test.Record.strings)
}
inline std::string* Record::_internal_add_strings() {
  return strings_.Add();
}
inline void Record::add_strings(const std::string& value) {
  strings_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:protobuf_test.Record.strings)
}
inline void Record::add_strings(std::string&& value) {
  strings_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:protobuf_test.Record.strings)
}
inline void Record::add_strings(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  strings_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:protobuf_test.Record.strings)
}
inline void Record::add_strings(const char* value, size_t size) {
  strings_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:protobuf_test.Record.strings)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
Record::strings() const {
  // @@protoc_insertion_point(field_list:protobuf_test.Record.strings)
  return strings_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
Record::mutable_strings() {
  // @@protoc_insertion_point(field_mutable_list:protobuf_test.Record.strings)
  return &strings_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf_test

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_test_2eproto
