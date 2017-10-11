// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_SERVER_WRITER_H
#define GRPC_CB_CORE_SERVER_WRITER_H

#include <string>

#include <grpc_cb_core/common/support/config.h>           // for GRPC_FINAL
#include <grpc_cb_core/common/call_sptr.h>  // for CallSptr

namespace grpc_cb_core {

class ServerWriterImpl;
class Status;

// ServerWriter for server-side streaming and bi-diretional streaming.
// Copyable. Thread-safe.
class ServerWriter GRPC_FINAL {
 public:
  explicit ServerWriter(const CallSptr& call_sptr);
  ~ServerWriter();

 public:
  bool Write(const std::string& response) const;
  bool SyncWrite(const std::string& response) const;
  void AsyncWrite(const std::string& response) const;

  size_t GetQueueSize() const;
  size_t GetHighQueueSize() const;
  void SetHighQueueSize(size_t high_queue_size);

  // Close() is optional. Dtr() will auto SyncClose().
  // Redundant close will be ignored.
  void SyncClose(const Status& status) const;
  void AsyncClose(const Status& status) const;
  bool IsClosed() const;

 private:
  const std::shared_ptr<ServerWriterImpl> impl_sptr_;
};  // class ServerWriter

}  // namespace grpc_cb_core

#endif  // GRPC_CB_CORE_SERVER_WRITER_H
