// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#ifndef LIBRBD_WATCH_NOTIFY_TYPES_H
#define LIBRBD_WATCH_NOTIFY_TYPES_H

#include "include/int_types.h"
#include "include/buffer.h"
#include "include/encoding.h"
#include <iosfwd>
#include <list>
#include <string>
#include <boost/variant.hpp>

namespace ceph {
class Formatter;
}

namespace librbd {
namespace WatchNotify {

struct ClientId {
  uint64_t gid;
  uint64_t handle;

  ClientId() : gid(0), handle(0) {}
  ClientId(uint64_t gid_, uint64_t handle_) : gid(gid_), handle(handle_) {}

  void encode(bufferlist& bl) const;
  void decode(bufferlist::iterator& it);
  void dump(Formatter *f) const;

  inline bool is_valid() const {
    return (*this != ClientId());
  }

  inline bool operator==(const ClientId &rhs) const {
    return (gid == rhs.gid && handle == rhs.handle);
  }
  inline bool operator!=(const ClientId &rhs) const {
    return !(*this == rhs);
  }
  inline bool operator<(const ClientId &rhs) const {
    if (gid != rhs.gid) {
      return gid < rhs.gid;
    } else {
      return handle < rhs.handle;
    }
  }
};

struct AsyncRequestId {
  ClientId client_id;
  uint64_t request_id;

  AsyncRequestId() : request_id() {}
  AsyncRequestId(const ClientId &client_id_, uint64_t request_id_)
    : client_id(client_id_), request_id(request_id_) {}

  void encode(bufferlist& bl) const;
  void decode(bufferlist::iterator& it);
  void dump(Formatter *f) const;

  inline bool operator<(const AsyncRequestId &rhs) const {
    if (client_id != rhs.client_id) {
      return client_id < rhs.client_id;
    } else {
      return request_id < rhs.request_id;
    }
  }
  inline bool operator!=(const AsyncRequestId &rhs) const {
    return (client_id != rhs.client_id || request_id != rhs.request_id);
  }
};

enum NotifyOp {
  NOTIFY_OP_ACQUIRED_LOCK      = 0,
  NOTIFY_OP_RELEASED_LOCK      = 1,
  NOTIFY_OP_REQUEST_LOCK       = 2,
  NOTIFY_OP_HEADER_UPDATE      = 3,
  NOTIFY_OP_ASYNC_PROGRESS     = 4,
  NOTIFY_OP_ASYNC_COMPLETE     = 5,
  NOTIFY_OP_FLATTEN            = 6,
  NOTIFY_OP_RESIZE             = 7,
  NOTIFY_OP_SNAP_CREATE        = 8,
  NOTIFY_OP_SNAP_REMOVE        = 9,
  NOTIFY_OP_REBUILD_OBJECT_MAP = 10,
  NOTIFY_OP_SNAP_RENAME = 11
};

struct AcquiredLockPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_ACQUIRED_LOCK;

  ClientId client_id;

  AcquiredLockPayload() {}
  AcquiredLockPayload(const ClientId &client_id_) : client_id(client_id_) {}

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct ReleasedLockPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_RELEASED_LOCK;

  ClientId client_id;

  ReleasedLockPayload() {}
  ReleasedLockPayload(const ClientId &client_id_) : client_id(client_id_) {}

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct RequestLockPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_REQUEST_LOCK;

  ClientId client_id;

  RequestLockPayload() {}
  RequestLockPayload(const ClientId &client_id_) : client_id(client_id_) {}

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct HeaderUpdatePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_HEADER_UPDATE;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct AsyncProgressPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_ASYNC_PROGRESS;

  AsyncProgressPayload() : offset(0), total(0) {}
  AsyncProgressPayload(const AsyncRequestId &id, uint64_t offset_, uint64_t total_)
    : async_request_id(id), offset(offset_), total(total_) {}

  AsyncRequestId async_request_id;
  uint64_t offset;
  uint64_t total;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct AsyncCompletePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_ASYNC_COMPLETE;

  AsyncCompletePayload() {}
  AsyncCompletePayload(const AsyncRequestId &id, int r)
    : async_request_id(id), result(r) {}

  AsyncRequestId async_request_id;
  int result;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct FlattenPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_FLATTEN;

  FlattenPayload() {}
  FlattenPayload(const AsyncRequestId &id) : async_request_id(id) {}

  AsyncRequestId async_request_id;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct ResizePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_RESIZE;

  ResizePayload() : size(0) {}
  ResizePayload(uint64_t size_, const AsyncRequestId &id)
    : size(size_), async_request_id(id) {}

  uint64_t size;
  AsyncRequestId async_request_id;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct SnapCreatePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_SNAP_CREATE;

  SnapCreatePayload() {}
  SnapCreatePayload(const std::string &name) : snap_name(name) {}

  std::string snap_name;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct SnapRenamePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_SNAP_RENAME;

  SnapRenamePayload() {}
  SnapRenamePayload(const uint64_t &src_snap_id, const std::string &dst_name)
    : src_snap_id(src_snap_id), dst_snap_name(dst_name) {}

  uint64_t src_snap_id;
  std::string dst_snap_name;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct SnapRemovePayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_SNAP_REMOVE;

  SnapRemovePayload() {}
  SnapRemovePayload(const std::string &name) : snap_name(name) {}

  std::string snap_name;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct RebuildObjectMapPayload {
  static const NotifyOp NOTIFY_OP = NOTIFY_OP_REBUILD_OBJECT_MAP;

  RebuildObjectMapPayload() {}
  RebuildObjectMapPayload(const AsyncRequestId &id) : async_request_id(id) {}

  AsyncRequestId async_request_id;

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

struct UnknownPayload {
  static const NotifyOp NOTIFY_OP = static_cast<NotifyOp>(-1);

  void encode(bufferlist &bl) const;
  void decode(__u8 version, bufferlist::iterator &iter);
  void dump(Formatter *f) const;
};

typedef boost::variant<AcquiredLockPayload,
                       ReleasedLockPayload,
                       RequestLockPayload,
                       HeaderUpdatePayload,
                       AsyncProgressPayload,
                       AsyncCompletePayload,
                       FlattenPayload,
                       ResizePayload,
                       SnapCreatePayload,
                       SnapRemovePayload,
                       SnapRenamePayload,
                       RebuildObjectMapPayload,
                       UnknownPayload> Payload;

struct NotifyMessage {
  NotifyMessage() : payload(UnknownPayload()) {}
  NotifyMessage(const Payload &payload_) : payload(payload_) {}

  Payload payload;

  void encode(bufferlist& bl) const;
  void decode(bufferlist::iterator& it);
  void dump(Formatter *f) const;

  static void generate_test_instances(std::list<NotifyMessage *> &o);
};

struct ResponseMessage {
  ResponseMessage() : result(0) {}
  ResponseMessage(int result_) : result(result_) {}

  int result;

  void encode(bufferlist& bl) const;
  void decode(bufferlist::iterator& it);
  void dump(Formatter *f) const;

  static void generate_test_instances(std::list<ResponseMessage *> &o);
};

} // namespace WatchNotify
} // namespace librbd

std::ostream &operator<<(std::ostream &out,
                         const librbd::WatchNotify::NotifyOp &op);
std::ostream &operator<<(std::ostream &out,
                         const librbd::WatchNotify::ClientId &client);
std::ostream &operator<<(std::ostream &out,
                         const librbd::WatchNotify::AsyncRequestId &request);

WRITE_CLASS_ENCODER(librbd::WatchNotify::ClientId);
WRITE_CLASS_ENCODER(librbd::WatchNotify::AsyncRequestId);
WRITE_CLASS_ENCODER(librbd::WatchNotify::NotifyMessage);
WRITE_CLASS_ENCODER(librbd::WatchNotify::ResponseMessage);

#endif // LIBRBD_WATCH_NOTIFY_TYPES_H
