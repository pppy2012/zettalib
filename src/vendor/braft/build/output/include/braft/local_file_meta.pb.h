// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: braft/local_file_meta.proto

#ifndef PROTOBUF_braft_2flocal_5ffile_5fmeta_2eproto__INCLUDED
#define PROTOBUF_braft_2flocal_5ffile_5fmeta_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_braft_2flocal_5ffile_5fmeta_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsLocalFileMetaImpl();
void InitDefaultsLocalFileMeta();
inline void InitDefaults() {
  InitDefaultsLocalFileMeta();
}
}  // namespace protobuf_braft_2flocal_5ffile_5fmeta_2eproto
namespace braft {
class LocalFileMeta;
class LocalFileMetaDefaultTypeInternal;
extern LocalFileMetaDefaultTypeInternal _LocalFileMeta_default_instance_;
}  // namespace braft
namespace braft {

enum FileSource {
  FILE_SOURCE_LOCAL = 0,
  FILE_SOURCE_REFERENCE = 1
};
bool FileSource_IsValid(int value);
const FileSource FileSource_MIN = FILE_SOURCE_LOCAL;
const FileSource FileSource_MAX = FILE_SOURCE_REFERENCE;
const int FileSource_ARRAYSIZE = FileSource_MAX + 1;

const ::google::protobuf::EnumDescriptor* FileSource_descriptor();
inline const ::std::string& FileSource_Name(FileSource value) {
  return ::google::protobuf::internal::NameOfEnum(
    FileSource_descriptor(), value);
}
inline bool FileSource_Parse(
    const ::std::string& name, FileSource* value) {
  return ::google::protobuf::internal::ParseNamedEnum<FileSource>(
    FileSource_descriptor(), name, value);
}
// ===================================================================

class LocalFileMeta : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:braft.LocalFileMeta) */ {
 public:
  LocalFileMeta();
  virtual ~LocalFileMeta();

  LocalFileMeta(const LocalFileMeta& from);

  inline LocalFileMeta& operator=(const LocalFileMeta& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  LocalFileMeta(LocalFileMeta&& from) noexcept
    : LocalFileMeta() {
    *this = ::std::move(from);
  }

  inline LocalFileMeta& operator=(LocalFileMeta&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const LocalFileMeta& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const LocalFileMeta* internal_default_instance() {
    return reinterpret_cast<const LocalFileMeta*>(
               &_LocalFileMeta_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(LocalFileMeta* other);
  friend void swap(LocalFileMeta& a, LocalFileMeta& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline LocalFileMeta* New() const PROTOBUF_FINAL { return New(NULL); }

  LocalFileMeta* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const LocalFileMeta& from);
  void MergeFrom(const LocalFileMeta& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(LocalFileMeta* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bytes user_meta = 1;
  bool has_user_meta() const;
  void clear_user_meta();
  static const int kUserMetaFieldNumber = 1;
  const ::std::string& user_meta() const;
  void set_user_meta(const ::std::string& value);
  #if LANG_CXX11
  void set_user_meta(::std::string&& value);
  #endif
  void set_user_meta(const char* value);
  void set_user_meta(const void* value, size_t size);
  ::std::string* mutable_user_meta();
  ::std::string* release_user_meta();
  void set_allocated_user_meta(::std::string* user_meta);

  // optional string checksum = 3;
  bool has_checksum() const;
  void clear_checksum();
  static const int kChecksumFieldNumber = 3;
  const ::std::string& checksum() const;
  void set_checksum(const ::std::string& value);
  #if LANG_CXX11
  void set_checksum(::std::string&& value);
  #endif
  void set_checksum(const char* value);
  void set_checksum(const char* value, size_t size);
  ::std::string* mutable_checksum();
  ::std::string* release_checksum();
  void set_allocated_checksum(::std::string* checksum);

  // optional .braft.FileSource source = 2;
  bool has_source() const;
  void clear_source();
  static const int kSourceFieldNumber = 2;
  ::braft::FileSource source() const;
  void set_source(::braft::FileSource value);

  // @@protoc_insertion_point(class_scope:braft.LocalFileMeta)
 private:
  void set_has_user_meta();
  void clear_has_user_meta();
  void set_has_source();
  void clear_has_source();
  void set_has_checksum();
  void clear_has_checksum();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr user_meta_;
  ::google::protobuf::internal::ArenaStringPtr checksum_;
  int source_;
  friend struct ::protobuf_braft_2flocal_5ffile_5fmeta_2eproto::TableStruct;
  friend void ::protobuf_braft_2flocal_5ffile_5fmeta_2eproto::InitDefaultsLocalFileMetaImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// LocalFileMeta

// optional bytes user_meta = 1;
inline bool LocalFileMeta::has_user_meta() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void LocalFileMeta::set_has_user_meta() {
  _has_bits_[0] |= 0x00000001u;
}
inline void LocalFileMeta::clear_has_user_meta() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void LocalFileMeta::clear_user_meta() {
  user_meta_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_user_meta();
}
inline const ::std::string& LocalFileMeta::user_meta() const {
  // @@protoc_insertion_point(field_get:braft.LocalFileMeta.user_meta)
  return user_meta_.GetNoArena();
}
inline void LocalFileMeta::set_user_meta(const ::std::string& value) {
  set_has_user_meta();
  user_meta_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:braft.LocalFileMeta.user_meta)
}
#if LANG_CXX11
inline void LocalFileMeta::set_user_meta(::std::string&& value) {
  set_has_user_meta();
  user_meta_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:braft.LocalFileMeta.user_meta)
}
#endif
inline void LocalFileMeta::set_user_meta(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_user_meta();
  user_meta_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:braft.LocalFileMeta.user_meta)
}
inline void LocalFileMeta::set_user_meta(const void* value, size_t size) {
  set_has_user_meta();
  user_meta_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:braft.LocalFileMeta.user_meta)
}
inline ::std::string* LocalFileMeta::mutable_user_meta() {
  set_has_user_meta();
  // @@protoc_insertion_point(field_mutable:braft.LocalFileMeta.user_meta)
  return user_meta_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* LocalFileMeta::release_user_meta() {
  // @@protoc_insertion_point(field_release:braft.LocalFileMeta.user_meta)
  clear_has_user_meta();
  return user_meta_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void LocalFileMeta::set_allocated_user_meta(::std::string* user_meta) {
  if (user_meta != NULL) {
    set_has_user_meta();
  } else {
    clear_has_user_meta();
  }
  user_meta_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), user_meta);
  // @@protoc_insertion_point(field_set_allocated:braft.LocalFileMeta.user_meta)
}

// optional .braft.FileSource source = 2;
inline bool LocalFileMeta::has_source() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void LocalFileMeta::set_has_source() {
  _has_bits_[0] |= 0x00000004u;
}
inline void LocalFileMeta::clear_has_source() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void LocalFileMeta::clear_source() {
  source_ = 0;
  clear_has_source();
}
inline ::braft::FileSource LocalFileMeta::source() const {
  // @@protoc_insertion_point(field_get:braft.LocalFileMeta.source)
  return static_cast< ::braft::FileSource >(source_);
}
inline void LocalFileMeta::set_source(::braft::FileSource value) {
  assert(::braft::FileSource_IsValid(value));
  set_has_source();
  source_ = value;
  // @@protoc_insertion_point(field_set:braft.LocalFileMeta.source)
}

// optional string checksum = 3;
inline bool LocalFileMeta::has_checksum() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void LocalFileMeta::set_has_checksum() {
  _has_bits_[0] |= 0x00000002u;
}
inline void LocalFileMeta::clear_has_checksum() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void LocalFileMeta::clear_checksum() {
  checksum_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_checksum();
}
inline const ::std::string& LocalFileMeta::checksum() const {
  // @@protoc_insertion_point(field_get:braft.LocalFileMeta.checksum)
  return checksum_.GetNoArena();
}
inline void LocalFileMeta::set_checksum(const ::std::string& value) {
  set_has_checksum();
  checksum_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:braft.LocalFileMeta.checksum)
}
#if LANG_CXX11
inline void LocalFileMeta::set_checksum(::std::string&& value) {
  set_has_checksum();
  checksum_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:braft.LocalFileMeta.checksum)
}
#endif
inline void LocalFileMeta::set_checksum(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_checksum();
  checksum_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:braft.LocalFileMeta.checksum)
}
inline void LocalFileMeta::set_checksum(const char* value, size_t size) {
  set_has_checksum();
  checksum_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:braft.LocalFileMeta.checksum)
}
inline ::std::string* LocalFileMeta::mutable_checksum() {
  set_has_checksum();
  // @@protoc_insertion_point(field_mutable:braft.LocalFileMeta.checksum)
  return checksum_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* LocalFileMeta::release_checksum() {
  // @@protoc_insertion_point(field_release:braft.LocalFileMeta.checksum)
  clear_has_checksum();
  return checksum_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void LocalFileMeta::set_allocated_checksum(::std::string* checksum) {
  if (checksum != NULL) {
    set_has_checksum();
  } else {
    clear_has_checksum();
  }
  checksum_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), checksum);
  // @@protoc_insertion_point(field_set_allocated:braft.LocalFileMeta.checksum)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace braft

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::braft::FileSource> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::braft::FileSource>() {
  return ::braft::FileSource_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_braft_2flocal_5ffile_5fmeta_2eproto__INCLUDED
