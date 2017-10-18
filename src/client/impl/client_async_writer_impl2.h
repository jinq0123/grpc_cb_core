// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL2_H
#define GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL2_H

#include <cstdint>  // for int64_t
#include <memory>  // for enable_shared_from_this<>
#include <mutex>
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb_core/client/close_cb.h>               // for CloseCb
#include <grpc_cb_core/common/call_sptr.h>              // for CallSptr
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb_core/common/status.h>                 // for Status
#include <grpc_cb_core/common/support/config.h>         // for GRPC_FINAL
#include "client_async_write_worker_sptr.h"  // for ClientAsyncWriteWorkerWptr

namespace grpc_cb_core {

class ClientAsyncWriteWorker;
class ClientWriterCloseCqTag;

// Impl of impl.
// Impl1 is to make Writer copyable.
// Impl2 will live longer than the Writer.
// We need dtr() of Impl1 to close writing.
// Thread-safe.
class ClientAsyncWriterImpl2 GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncWriterImpl2> {
 public:
  ClientAsyncWriterImpl2(const ChannelSptr& channel, const std::string& method,
                         const CompletionQueueSptr& cq_sptr, int64_t timeout_ms);
  ~ClientAsyncWriterImpl2();

  bool Write(const std::string& request);
  void Close(const CloseCb& close_cb = nullptr);

 private:
  // for ClientWriterCloseCqTag::OnComplete()
  void OnClosed(bool success, ClientWriterCloseCqTag& tag);
  void OnEndOfWriting();  // Callback from WriteWorker

  // Todo: Force to close, cancel all writing.
  // Todo: get queue size

 private:
  void SendCloseIfNot();
  void CallCloseCb(const std::string& sMsg = "");
  void SetInternalError(const std::string& sError);

 private:
  // The callback may lock the mutex recursively.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CompletionQueueSptr cq_sptr_;
  const CallSptr call_sptr_;
  Status status_;
  bool has_sent_close_ = false;  // Client send close once.
  CloseCb close_cb_;

  bool writing_started_ = false;  // Set by Write()
  bool writing_closing_ = false;  // Set by Close()
  bool writing_ended_ = false;  // all written?

  // Use weak_ptr to avoid share loop.
  ClientAsyncWriteWorkerWptr writer_wptr_;
  ClientAsyncWriteWorkerSptr writer_sptr_;  // Keep until Close()
};  // class ClientAsyncWriterImpl2

}  // namespace grpc_cb_core
#endif  // GRPC_CB_CORE_CLIENT_ASYNC_WRITER_IMPL2_H
