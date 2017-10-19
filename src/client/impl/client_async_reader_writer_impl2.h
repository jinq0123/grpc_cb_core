// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H
#define GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H

#include <cassert>
#include <cstdint>  // for int64_t
#include <memory>  // for enable_shared_from_this<>
#include <mutex>
#include <string>
#include <queue>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/msg_str_cb.h>             // for MsgStrCb
#include <grpc_cb_core/client/status_cb.h>              // for StatusCb
#include <grpc_cb_core/common/call_sptr.h>              // for CallSptr
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/status.h>                 // for Status
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL

namespace grpc_cb_core {

class ClientReaderReadCqTag;

// Impl of impl.
// Impl1 is to make Writer copyable.
// Impl2 will live longer than the Writer.
// We need dtr() of Impl1 to close writing.
// Thread-safe.
class ClientAsyncReaderWriterImpl2 GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderWriterImpl2> {
 public:
  ClientAsyncReaderWriterImpl2(const ChannelSptr& channel,
                               const std::string& method,
                               const CompletionQueueSptr& cq_sptr,
                               int64_t timeout_ms,
                               const StatusCb& status_cb);
  ~ClientAsyncReaderWriterImpl2();

 public:
  bool Write(const std::string& msg);
  void CloseWriting();

  // Todo: Force to close reading/writing. Cancel all reading/writing.

  void ReadEach(const MsgStrCb& msg_cb);

  // Set error status to break reading. Such as when parsing message failed.
  void SetErrorStatus(const Status& error_status);

 private:
  // For ClientSendMsgCqTag.
  void OnSent(bool success);
  // For ClientReaderReadCqTag.
  void OnRead(bool success, ClientReaderReadCqTag& tag);

 private:
  void InitIfNot();
  bool SendNext();
  void ReadNext();
  void SendClose();
  void SetInternalError(const std::string& sError);
  void InternalSetErrorStatus(const Status& error_status);
  void CallStatusCb();

 private:
  // Callbacks will lock again.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CallSptr call_sptr_;
  Status status_;
  MsgStrCb msg_cb_;
  StatusCb status_cb_;
  bool inited_ = false;  // Init once.
  bool reading_started_ = false;  // Set by ReadEach()
  bool writing_closing_ = false;  // Set by CloseWriting()
  bool reading_ended_ = false;
  bool writing_ended_ = false;
  std::queue<std::string> msg_queue_;  // Cache messages to write.

#ifndef NDEBUG
  bool has_sent_close_ = false;  // Client send close once.
#endif  // NDEBUG
};  // class ClientAsyncReaderWriterImpl2

// Todo: SyncGetInitMd();

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H
